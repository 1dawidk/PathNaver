cmake_minimum_required(VERSION 3.18)
project(PathNaver)

set(CMAKE_CXX_STANDARD 17)
include_directories(SuperMisc SuperSerial)

add_definitions(-DDEBUG)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWS2811 REQUIRED libws2811)
find_package(Threads REQUIRED)
find_library(bluetooth_lib bluetooth REQUIRED)
MESSAGE( STATUS "bluetooth Library: " ${bluetooth_lib} )

add_executable(PathNaver main.cpp SuperSerial/Serial.cpp SuperSerial/Serial.h SuperSerial/SerialBuffer.cpp SuperSerial/SerialBuffer.h SuperSerial/SuperSerial.cpp SuperSerial/SuperSerial.h SuperSerial/NMEA/NMEA.cpp SuperSerial/NMEA/NMEA.h GNSSModule.cpp GNSSModule.h SuperMisc/SuperMisc.cpp SuperMisc/SuperMisc.h SuperMisc/tim.h SuperMisc/tim.cpp FlightPath.cpp FlightPath.h Geo/GeoPoint.cpp Geo/GeoPoint.h Geo/GeoCalc.cpp Geo/GeoCalc.h Geo/GeoRoute.cpp Geo/GeoRoute.h UI/LedGUI.cpp UI/LedGUI.h UI/WS281xStrip.cpp UI/WS281xStrip.h Naver.cpp Naver.h Communication/TCPSerialComm.cpp Communication/TCPSerialComm.h Communication/BufferedReader.cpp Communication/BufferedReader.h Console.cpp Console.h Communication/Talker.cpp Communication/Talker.h Worker.cpp Worker.h KMLWatcher.cpp KMLWatcher.h DeviceConfig.cpp DeviceConfig.h)
target_link_libraries(PathNaver ${LIBWS2811_LIBRARIES} Threads::Threads ${bluetooth_lib})