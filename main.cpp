#include <iostream>
#include <thread>
#include <filesystem>

#include <tinyb.hpp>
#include "global_ble/SwitchBotBluetoothPeripheral.hpp"
#include "global_ble/BluetoothSupervisor.hpp"
#include "MqttPublisher.hpp"
#include "JsonSpecificationReader.hpp"


using namespace tinyb;


int main(int argc, char **argv)
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path defPath = currentPath / "home_definition.json";

    if (!std::filesystem::exists(defPath)) {
        defPath = currentPath / ".." / "home_definition.json";
        if (!std::filesystem::exists(defPath)) {
            std::cerr << "definition file not found. Backup to know location";
            exit(1);
        }
    }
    std::cout << "Found definition file " << defPath.string() << "\n";
    JsonSpecificationReader jsonSpecReader(defPath);

    global_ble::BluetoothSupervisor bleSupervisor(jsonSpecReader.getRoomToPeripherals());

    bleSupervisor.init();

    // connect 
    mqtt::MqttPublisher mqttPublisher(jsonSpecReader.getMqttServer(), jsonSpecReader.getMqttClientId());
    
    do {
        if (!mqttPublisher.connect("domoticz/in")) {
            std::cerr << "Error while activating mqtt\n";
        } else {
            auto futureRslt = bleSupervisor.startScanAsync();
            futureRslt.wait();

            auto periphs = bleSupervisor.getPeripherals();
            for (auto const& p : periphs) {
                if (p->fType == global_ble::BluetoothPeripheralType::SwitchBot) {
                    SwitchBotBluetoothPeripheral* pSwitchBot = dynamic_cast<SwitchBotBluetoothPeripheral*>(p.get());
                    std::cout << pSwitchBot->toString();
                    if (pSwitchBot->hasData()) {
                        auto const& bleSwitchBot = *p.get();
                        mqttPublisher.publish(std::to_string(bleSwitchBot.fDomoticzIdx), bleSwitchBot.getDomoticzSValue());
                    }
                }
            }
            mqttPublisher.disconnect();
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5min);
        std::cerr << "Sleeping\n";
    } while(1);

    
}
