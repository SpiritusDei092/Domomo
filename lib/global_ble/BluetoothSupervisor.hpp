#pragma once

#include "spec.hpp"

#include <iostream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <future>
#include <tinyb.hpp>

#include "BluetoothPeripheral.hpp"
namespace global_ble {

class LIBFOO_SHLIB_EXPORTED BluetoothSupervisor {
public:
    BluetoothSupervisor(std::unordered_map<std::string, std::vector<std::shared_ptr<BluetoothPeripheral>>> roomToPeripherals)
        : fRoomToPeripherals(roomToPeripherals) {}

    void init();
    std::future<bool> startScanAsync();
    std::shared_ptr<BluetoothPeripheral> getPeripheral(std::string const& bleAddress) ;
    std::vector<std::shared_ptr<BluetoothPeripheral>> getPeripherals(std::string const& roomFilter = "") const;
private:
    bool asyncScan();

    std::thread fScanThread;
    std::unordered_map<std::string, std::vector<std::shared_ptr<BluetoothPeripheral>>> fRoomToPeripherals;
    std::filesystem::path fJsonDefinitionFile;
    BluetoothManager* fpBleManager;
};
}
