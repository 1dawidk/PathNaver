cmake_minimum_required(VERSION 3.18)
project(PathNaver)

set(CMAKE_CXX_STANDARD 17)
include_directories(SuperMisc SuperSerial)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWS2811 REQUIRED libws2811)
find_package(Threads REQUIRED)

add_executable(PathNaver main.cpp SuperSerial/Serial.cpp SuperSerial/Serial.h SuperSerial/SerialBuffer.cpp SuperSerial/SerialBuffer.h SuperSerial/SuperSerial.cpp SuperSerial/SuperSerial.h SuperSerial/NMEA/NMEA.cpp SuperSerial/NMEA/NMEA.h GNSSModule.cpp GNSSModule.h SuperMisc/SuperMisc.cpp SuperMisc/SuperMisc.h SuperMisc/tim.h SuperMisc/tim.cpp FlightPath.cpp FlightPath.h GeoPoint.cpp GeoPoint.h Geo.cpp Geo.h GeoRoute.cpp GeoRoute.h LedGUI.cpp LedGUI.h WS281xStrip.cpp WS281xStrip.h)
target_link_libraries(PathNaver ${LIBWS2811_LIBRARIES} Threads::Threads)