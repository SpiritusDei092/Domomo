#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "global_ble/BluetoothPeripheral.hpp"

#include "spec.hpp"

class LIBFOO_SHLIB_EXPORTED JsonSpecificationReader {
public:
    JsonSpecificationReader(std::filesystem::path const& jsonDefinitionFile);
    std::string getMqttServer() const { return fMqttServer; };
    std::string getMqttClientId() const { return fMqttClientId; };
    std::unordered_map<std::string, std::vector<std::shared_ptr<global_ble::BluetoothPeripheral>>> getRoomToPeripherals() const { return fRoomToPeripherals; };
private:
    void readJsonSpecification(std::filesystem::path const& jsonDefinitionFile);

    std::string fMqttServer;
    std::string fMqttClientId;
    std::vector<std::string> rooms;
    std::unordered_map<std::string, std::vector<std::shared_ptr<global_ble::BluetoothPeripheral>>> fRoomToPeripherals;

};