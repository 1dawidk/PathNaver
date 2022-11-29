#include <iostream>
#include <thread>
#include "args-parser/all.hpp"
#include "Navigation/GNSSModule.h"
#include "Navigation/FlightPath.h"
#include "UI/LedGUI.h"
#include "Navigation/Naver.h"
#include <csignal>
#include "Console.h"
#include "Navigation/KMLWatcher.h"
#include "Communication/Talker.h"

#define COUNTDOWN_START_DISTANCE        300.0
#define COUNTDOWN_STEP                  100.0
#define FINISH_BLINK_START_DISTANCE     40.0
#define OFFSET_MAX                      150

bool runApp = true;

struct sigaction sigIntHandler{};
struct sigaction sigTermHandler{};
struct sigaction sigPipeHandler{};

void configCLI(Args::CmdLine &cmd, char* call);
void configSigCatch();
void stopApp(int s);

int main(int argc, char** argv) {
    // Prepare signal catchers
    configSigCatch();

    // Prepare command line arguments parser
    Args::CmdLine cmd( argc, argv );
    configCLI(cmd, argv[0]);

    // Prepare custom console log system
    Console::start();

    // Try to parse args
    try {
        cmd.parse();
    } catch (const Args::HelpHasBeenPrintedException &e){
        std::cout << e.desc() << std::endl;
        Console::stop();
        return 0;
    } catch (const Args::BaseException &e){
        std::cout << e.desc() << std::endl;
        Console::stop();
        return 0;
    }

    // Probe verbose arg
    bool verbose = cmd.isDefined("-v");

    // Probe device arg
    std::string device;
    if(cmd.isDefined("-d"))
        device = cmd.value("-d");

    /**
     * Should run threads for:
     * - GNSS Module process    |   +
     * - Navigator              |   +
     * - LED GUI                |   +
     * - KMLs watching process  |   +
     * - Communication process  |   +
     * - TSC                    |   +
     * - MAIN THREAD            |   +
     */

    // Prepare led string gui
    LedGUI gui(39, OFFSET_MAX); // Leds number and max shift in meters
    gui.start();

    // Create Workers
    GNSSModule gnss(std::stoi(cmd.value("-b")), verbose, device);
    Naver naver(&gnss);
    WirelessSerialComm wsc;
    DeviceConfig loadedDeviceConfig;
    DeviceConfig deviceConfig;
    KMLWatcher kmlWatcher;
    Talker talker(&wsc, &kmlWatcher, &deviceConfig, &naver);

    // Start threads
    gnss.start();
    naver.start();
    wsc.start();
    kmlWatcher.start();
    talker.start();

    // Start bluetooth pairable process
    Console::logd("main", "Showing Bluetooth discoverable GUI mode");
    wsc.makePairable();
    gui.setMode(LedGUI::MODE_BTDISCOVERABLE);

    // Clear loaded path id (-1) and pause naver
    loadedDeviceConfig.setSelectedPathId(-1);
    naver.pause();

    /*** MAIN LOOP ***/
    while(runApp){
        if(wsc.isPairable()){

        } else {
            // If path has changed load it
            int pathId= deviceConfig.getSelectedPathId();
            if(pathId>0 && pathId<kmlWatcher.countFound() && loadedDeviceConfig.getSelectedPathId()!=pathId){
                loadedDeviceConfig.setSelectedPathId(pathId);
                auto *fp= new FlightPath(kmlWatcher.getFlightPath(pathId));
                naver.pause();
                naver.loadPath(fp, pathId);
                naver.resume();
            }

            // Naver is prepared and can run (GPS fixed)
            if(naver.hasFix() && naver.isPathLoaded()){
                if(gui.getMode()!=LedGUI::MODE_NAV){
                    gui.setMode(LedGUI::MODE_NAV);
                    naver.resume();
                }

                // Update gui
                gui.setRouteShift(naver.getShift());
                gui.setNavCountdown(static_cast<char>(naver.getNavCountdown()));
                if(naver.getPassedEndFlag())
                    gui.blinkFinish();
            } else {
                if(gui.getMode()!=LedGUI::MODE_CAROUSEL){
                    gui.setMode(LedGUI::MODE_CAROUSEL);
                    naver.pause();
                }
            }
        }

        usleep(1000); // 1ms
    }

    Console::logi("main", "Stopping threads...");
    naver.stop();
    gnss.stop();
    kmlWatcher.stop();
    talker.stop();
    gui.stop();
    wsc.stop();
    Console::logi("main", "Workers stopped");
    Console::stop();
    std::cout << "Console stopped! Bye, bye!" << std::endl;

    return 0;
}

void configCLI(Args::CmdLine &cmd, char* call){
    cmd.addArgWithFlagAndName( 'b', "baudrate", true, true, "serial baudrate", "sets serial baudrate", "", "VAL")
            .addArgWithFlagAndName( 'd', "device", true, false, "serial port", "device's serial port", "", "PATH" )
            .addArgWithFlagAndName( 'i', "input", true, false, "input file path", " input kml file path with defined flight route", "", "PATH" )
            .addArgWithFlagAndName('v', "verbose", false, false, "verbose",  "increase verbose level")
            .addHelp( true, call, "Path Naver" );
}

void configSigCatch(){
    // *** Catch INTERRUPT signal ***
    sigIntHandler.sa_handler = stopApp;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
    // ******************************

    // *** Catch TERMINATE signal ***
    sigTermHandler.sa_handler = stopApp;
    sigemptyset(&sigTermHandler.sa_mask);
    sigTermHandler.sa_flags = 0;

    sigaction(SIGTERM, &sigTermHandler, nullptr);
    // ******************************

    // *** Catch PIPE signal ***
    sigPipeHandler.sa_handler = stopApp;
    sigemptyset(&sigPipeHandler.sa_mask);
    sigPipeHandler.sa_flags = 0;

    sigaction(SIGPIPE, &sigPipeHandler, nullptr);
    // ******************************
}

void stopApp([[maybe_unused]] int s){
    std::cout << "Caught sig " << s << std::endl;
    runApp = false;
}