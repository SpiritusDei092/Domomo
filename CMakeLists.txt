cmake_minimum_required(VERSION 3.0)

project (Domomo)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

enable_testing()

link_libraries(libtinyb.so)
link_libraries(paho-mqttpp3)
link_libraries(paho-mqtt3as)
link_libraries(pthread)
find_package(CURL REQUIRED)

set(LIB_SOURCES
    lib/JsonSpecificationReader.cpp
    lib/MqttPublisher.cpp
    lib/global_ble/BluetoothSupervisor.cpp
)

add_library(domomolib ${LIB_SOURCES})
target_compile_definitions(domomolib PRIVATE BUILDING_LIBDOMOMO)
target_include_directories(domomolib PUBLIC lib)

# first app
add_executable(Domomo main.cpp)
target_link_libraries(Domomo PRIVATE domomolib)

# Second app
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Linky")
add_executable(Linky Linky/Linky.cpp)
target_link_libraries(Linky CURL::libcurl domomolib)
endif()

### Polyspace Test 

# find pstest package
find_package(pstest 25.1 QUIET)




