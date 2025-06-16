#pragma once

#include <vector>
#include <iostream>
#include <stdint.h>
#include <sstream>
#include <string_view>

#include "BluetoothSupervisor.hpp"

#define SWITCHBOT_METHER_TH    0x54

class LIBFOO_SHLIB_EXPORTED SwitchBotBluetoothPeripheral : public global_ble::BluetoothPeripheral {
public:
    constexpr static std::string_view fTypeName{"SwitchBot"};
    SwitchBotBluetoothPeripheral(std::string room,
                     std::string name,
                     std::string address,
                     int domoticzIdx,
                     std::string mqttFormat) : BluetoothPeripheral(room, name, address, fTypeName.data(), domoticzIdx) {}

    void setData(std::vector<uint8_t> const& manufacturerData) override {
        if ((manufacturerData.size() >= 11)) {
            uint8_t decimalT = manufacturerData[8] & 0x0F;
            uint16_t T = manufacturerData[9] & 0x7F;
            if ((manufacturerData[9] & 0x80) == 0) {
                T = -T;
            }
            fTemperature = (float)((T * 10) + decimalT) / 10;
            fHumidity = manufacturerData[10] & 0x7F;
        }
    }

    std::string getTemperature() const {
        std::stringstream val;
        val << "Temperature: " << fTemperature << "°C";
        return val.str();
    }

    std::string getHumidity() const {
        std::stringstream val;
        val << "Humidity: " << static_cast<unsigned>(fHumidity) << "%";
        return val.str();
    }

    std::string toString() const override {
        std::stringstream val;
        val << fRoom << " "<< fName << "\n";
        val << "\tTemperature: " << fTemperature << "°C\n";
        val << "\tHumidity: " << static_cast<unsigned>(fHumidity) << "%\n";
        // val << "\tBattery: " << static_cast<unsigned>(fBattery) << "%\n";
        return val.str();
    }

    std::string getDomoticzSValue() const override {
        char buffer[50];
        sprintf(buffer, "%2.1f;%u;1", fTemperature, fHumidity);
        return std::string(buffer);
    }

    bool hasData() const override {
        return (fTemperature != 0) && (fHumidity != 0);
    }

private:
    uint8_t status;
    // uint8_t fBattery;
    float fTemperature;
    uint8_t fHumidity;
};