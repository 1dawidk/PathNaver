cmake_minimum_required(VERSION 3.18)
project(PathNaver)

set(CMAKE_CXX_STANDARD 17)
include_directories(SuperMisc SuperSerial)

add_definitions(-DDEBUG)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWS2811 REQUIRED libws2811)
find_package(Threads REQUIRED)
find_library(bluetooth_lib bluetooth REQUIRED)

add_executable(PathNaver main.cpp SuperSerial/Serial.cpp SuperSerial/Serial.h SuperSerial/SerialBuffer.cpp SuperSerial/SerialBuffer.h SuperSerial/SuperSerial.cpp SuperSerial/SuperSerial.h SuperSerial/NMEA/NMEA.cpp SuperSerial/NMEA/NMEA.h navigation/GNSSModule.cpp navigation/GNSSModule.h SuperMisc/SuperMisc.cpp SuperMisc/SuperMisc.h SuperMisc/tim.h SuperMisc/tim.cpp navigation/FlightPath.cpp navigation/FlightPath.h geo/GeoPoint.cpp geo/GeoPoint.h geo/GeoCalc.cpp geo/GeoCalc.h geo/GeoRoute.cpp geo/GeoRoute.h ui/LedGUI.cpp ui/LedGUI.h ui/WS281xStrip.cpp ui/WS281xStrip.h navigation/Naver.cpp navigation/Naver.h communication/TCPApi.cpp communication/TCPApi.h communication/BufferedReader.cpp communication/BufferedReader.h Console.cpp Console.h communication/Talker.cpp communication/Talker.h SuperMisc/Worker.cpp SuperMisc/Worker.h navigation/KMLWatcher.cpp navigation/KMLWatcher.h DeviceConfig.cpp DeviceConfig.h navigation/GNSSData.cpp navigation/GNSSData.h SuperSerial/NMEA/NMEAData.cpp SuperSerial/NMEA/NMEAData.h communication/BluetoothApi.cpp communication/BluetoothApi.h communication/WirelessSerialComm.cpp communication/WirelessSerialComm.h navigation/KMLFile.cpp navigation/KMLFile.h)
target_link_libraries(PathNaver ${LIBWS2811_LIBRARIES} Threads::Threads ${bluetooth_lib})