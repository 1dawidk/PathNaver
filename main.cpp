#include <iostream>
#include <thread>
#include "args-parser/all.hpp"
#include "GNSSModule.h"
#include "FlightPath.h"
#include "UI/LedGUI.h"
#include "Naver.h"
#include <csignal>
#include "Communication/Bluetooth.h"
#include "Communication/TCPSerialComm.h"

#define COUNTDOWN_START_DISTANCE        300.0
#define COUNTDOWN_STEP                  100.0
#define FINISH_BLINK_START_DISTANCE     40.0
#define OFFSET_MAX                      150

void configCLI(Args::CmdLine &cmd, char* call);

bool runApp = true;
void int_handler(int s){
    printf("Caught signal %d\n",s);
    runApp = false;
}

int main(int argc, char** argv) {
    // *** Catch interrupt signal ***
    struct sigaction sigIntHandler{};

    sigIntHandler.sa_handler = int_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);
    // ******************************

    Args::CmdLine cmd( argc, argv );
    configCLI(cmd, argv[0]);

    // Try to parse args
    try {
        cmd.parse();
    } catch (const Args::HelpHasBeenPrintedException &e){
        std::cout << e.desc() << std::endl;
        return 0;
    } catch (const Args::BaseException &e){
        std::cout << e.desc() << std::endl;
        return 0;
    }

    // Empty flight path object
    FlightPath *fp;

    // Prepare led string
    LedGUI gui(39, OFFSET_MAX); // Leds number and max shift in meters

    // Prepare bluetooth interface
    TCPSerialComm sc;
    sc.start();

    // Probe verbose arg
    bool verbose = cmd.isDefined("-v");

    // Probe device arg
    std::string device;
    if(cmd.isDefined("-d"))
        device = cmd.value("-d");

    // Create and init eMLID device
    auto gnss = GNSSModule(std::stoi(cmd.value("-b")), verbose, device);

    /*** MAIN LOOP ***/
    std::thread uiThd(&LedGUI::exec, &gui);
    std::thread scThd(&TCPSerialComm::exec, &sc);

    const int APP_STATE_INIT_PATH = 1;
    const int APP_STATE_NAVIGATION = 3;

    int appState = APP_STATE_INIT_PATH;

    Naver naver(&gui, &gnss);

    while(runApp){
        switch (appState) { // NOLINT(hicpp-multiway-paths-covered)
            case APP_STATE_INIT_PATH:{
                gui.setMode(LedGUI::MODE_INIT);
                // Determine KML file path
                std::string kmlFilePath;
                if(cmd.isDefined("-i")){
                    kmlFilePath= cmd.value("-i");
                } else {
                    kmlFilePath= FlightPath::findKMLOnDrive();
                }
                if(!kmlFilePath.empty()) {
                    try {
                        // std::cout << "[INFO] KML search result: " << kmlFilePath << std::endl;
                        fp = new FlightPath(kmlFilePath);
                        naver.loadPath(fp);
                        appState = APP_STATE_NAVIGATION;
                        fp->print();
                    } catch (const std::runtime_error &e) {
                        // std::cout << e.what() << std::endl;
                        usleep(1000000);
                    }
                } else {
                    std::cout << "[ERR] No KML file defined nor found!" << std::endl;
                    usleep(1000000);
                }
                break;
            }
            case APP_STATE_NAVIGATION:{
                naver.update();
                break;
            }
        }

        usleep(100000); // 100ms
    }

    gui.stop();
}

void configCLI(Args::CmdLine &cmd, char* call){
    cmd.addArgWithFlagAndName( 'b', "baudrate", true, true, "serial baudrate", "sets serial baudrate", "", "VAL")
            .addArgWithFlagAndName( 'd', "device", true, false, "serial port", "device's serial port", "", "PATH" )
            .addArgWithFlagAndName( 'i', "input", true, false, "input file path", " input kml file path with defined flight route", "", "PATH" )
            .addArgWithFlagAndName('v', "verbose", false, false, "verbose",  "increase verbose level")
            .addHelp( true, call, "Path Naver" );
}