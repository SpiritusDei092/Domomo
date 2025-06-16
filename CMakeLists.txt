cmake_minimum_required(VERSION 3.0)

project (SwitchBotBLE)

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
add_executable(SwitchBotBle main.cpp)
target_link_libraries(SwitchBotBle PRIVATE domomolib)

# Second app
add_executable(Linky Linky.cpp)
target_link_libraries(Linky CURL::libcurl domomolib)

### Polyspace Test 

# find pstest package
find_package(pstest 25.1 QUIET)




