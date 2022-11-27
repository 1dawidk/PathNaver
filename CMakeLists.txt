cmake_minimum_required(VERSION 3.18)
project(PathNaver)

set(CMAKE_CXX_STANDARD 17)
include_directories(SuperMisc SuperSerial)

add_definitions(-DDEBUG)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWS2811 REQUIRED libws2811)
find_package(Threads REQUIRED)
find_library(bluetooth_lib bluetooth REQUIRED)

add_executable(PathNaver main.cpp SuperSerial/Serial.cpp SuperSerial/Serial.h SuperSerial/SerialBuffer.cpp SuperSerial/SerialBuffer.h SuperSerial/SuperSerial.cpp SuperSerial/SuperSerial.h SuperSerial/NMEA/NMEA.cpp SuperSerial/NMEA/NMEA.h Navigation/GNSSModule.cpp Navigation/GNSSModule.h SuperMisc/SuperMisc.cpp SuperMisc/SuperMisc.h SuperMisc/tim.h SuperMisc/tim.cpp Navigation/FlightPath.cpp Navigation/FlightPath.h Geo/GeoPoint.cpp Geo/GeoPoint.h Geo/GeoCalc.cpp Geo/GeoCalc.h Geo/GeoRoute.cpp Geo/GeoRoute.h UI/LedGUI.cpp UI/LedGUI.h UI/WS281xStrip.cpp UI/WS281xStrip.h Navigation/Naver.cpp Navigation/Naver.h Communication/TCPApi.cpp Communication/TCPApi.h Communication/BufferedReader.cpp Communication/BufferedReader.h Console.cpp Console.h Communication/Talker.cpp Communication/Talker.h SuperMisc/Worker.cpp SuperMisc/Worker.h Navigation/KMLWatcher.cpp Navigation/KMLWatcher.h DeviceConfig.cpp DeviceConfig.h Navigation/GNSSData.cpp Navigation/GNSSData.h SuperSerial/NMEA/NMEAData.cpp SuperSerial/NMEA/NMEAData.h Communication/BluetoothApi.cpp Communication/BluetoothApi.h Communication/WirelessSerialComm.cpp Communication/WirelessSerialComm.h)
target_link_libraries(PathNaver ${LIBWS2811_LIBRARIES} Threads::Threads ${bluetooth_lib})