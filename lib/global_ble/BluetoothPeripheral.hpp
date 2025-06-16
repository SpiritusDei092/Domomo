
#pragma once
#include <iostream>
#include "spec.hpp"
namespace global_ble {

enum class BluetoothPeripheralType {
    Unknown,
    SwitchBot
};

struct LIBFOO_SHLIB_EXPORTED BluetoothPeripheral {
    std::string fRoom;
    std::string fAddress;
    std::string fName;
    BluetoothPeripheralType fType;
    int fDomoticzIdx;
    BluetoothPeripheral(std::string room,
               std::string name,
               std::string address,
               std::string type,
               int domoticzIdx): fAddress(address), fName(name), fRoom(room), fDomoticzIdx(domoticzIdx) {
        if (type == "SwitchBot") {
            fType = BluetoothPeripheralType::SwitchBot;
        }
    }

    virtual void setData(std::vector<uint8_t> const& advertisingData) {};
    virtual std::string getDomoticzSValue() const = 0;
    virtual bool hasData() const = 0;
    virtual std::string toString() const {
        return "\n\tRoom: " + fRoom + "\n\tName: " + fName + "\n\tAddress: " + fAddress;
    }
};
}