#include <iostream>
#include <thread>
#include "args-parser/all.hpp"
#include "GNSSModule.h"
#include "FlightPath.h"
#include "LedGUI.h"

#define COUNTDOWN_START_DISTANCE        300.0
#define COUNTDOWN_STEP                  100.0
#define FINISH_BLINK_START_DISTANCE     40.0
#define OFFSET_MAX                      150

void configCLI(Args::CmdLine &cmd, char* call);

int main(int argc, char** argv) {

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


    // Probe verbose arg
    bool verbose = cmd.isDefined("-v");

    // Probe device arg
    std::string device;
    if(cmd.isDefined("-d"))
        device = cmd.value("-d");

    // Create and init eMLID device
    auto gnss = GNSSModule(std::stoi(cmd.value("-b")), verbose, device);

    /*** MAIN LOOP ***/
    std::thread thd(&LedGUI::exec, &gui);

    const int APP_STATE_INIT_PATH = 1;
    const int APP_STATE_NAVIGATION = 3;

    int appState = APP_STATE_INIT_PATH;

    int blinkedFor= -1;

    unsigned long lastMsgAt= tim::now();

    while(true){
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
                        std::cout << "[INFO] KML search result: " << kmlFilePath << std::endl;
                        fp = new FlightPath(kmlFilePath);
                        appState = APP_STATE_NAVIGATION;
                        fp->print();
                    } catch (const std::runtime_error &e) {
                        std::cout << "[ERR] " << e.what() << std::endl;
                        usleep(1000000);
                    }
                } else {
                    std::cout << "[ERR] No KML file defined nor found!" << std::endl;
                    usleep(1000000);
                }
                break;
            }
            case APP_STATE_NAVIGATION:{
                if(!gnss.isOpen()){
                    gui.setMode(LedGUI::MODE_CAROUSEL);
                    gnss.cleanup();
                    gnss.startBuffer();
                } else {
                    gnss.update();
                    if (gnss.hasFix()) {
                        gui.setMode(LedGUI::MODE_NAV);
                        GeoPoint me(gnss.getLat(), gnss.getLng());
                        int my_path_idx = fp->findMyPath(me, gnss.getHeading());
                        GeoRoute my_path = fp->getRoute(my_path_idx);
                        double shift = my_path.distanceTo(me) * (double)my_path.getShiftDirection(me);
                        gui.setRouteShift(shift);

                        if(my_path.distanceToEnd(me) < COUNTDOWN_START_DISTANCE) {
                            if(blinkedFor != my_path_idx) {
                                if (my_path.distanceToEnd(me) < FINISH_BLINK_START_DISTANCE) {
                                    gui.blinkFinish();
                                    blinkedFor = my_path_idx;
                                } else {
                                    gui.setNavCountdown((char)((my_path.distanceToEnd(me)/COUNTDOWN_STEP)+1));
                                }
                            }
                        }

                        if(lastMsgAt+1000 < tim::now()) {
                            std::cout << "My route: " << my_path_idx << ", offset: " << my_path.distanceTo(me) << "m, "
                                      << my_path.distanceToEnd(me)
                                      << "m left" << std::endl;
                            lastMsgAt = tim::now();
                        }
                    } else {
                        gui.setMode(LedGUI::MODE_CAROUSEL);
                    }
                }
                break;
            }
        }

        usleep(100000); // 100ms
    }
}

void configCLI(Args::CmdLine &cmd, char* call){
    cmd.addArgWithFlagAndName( 'b', "baudrate", true, true, "serial baudrate", "sets serial baudrate", "", "VAL")
            .addArgWithFlagAndName( 'd', "device", true, false, "serial port", "device's serial port", "", "PATH" )
            .addArgWithFlagAndName( 'i', "input", true, false, "input file path", " input kml file path with defined flight route", "", "PATH" )
            .addArgWithFlagAndName('v', "verbose", false, false, "verbose",  "increase verbose level")
            .addHelp( true, call, "Path Naver" );
}