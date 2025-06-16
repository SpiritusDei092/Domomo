#include <thread>
#include <string_view>
#include "SwitchBotBluetoothPeripheral.hpp"
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


#include "BluetoothSupervisor.hpp"


namespace global_ble {

std::vector<std::shared_ptr<BluetoothPeripheral>> BluetoothSupervisor::getPeripherals(std::string const& roomFilter) const {
    std::vector<std::shared_ptr<BluetoothPeripheral>> periph;
    if (roomFilter.empty()) {
        for (auto const& [room, peripherals] : fRoomToPeripherals) {
            periph.insert(periph.end(), peripherals.begin(), peripherals.end());
        }
    } else {
        auto it = fRoomToPeripherals.find(roomFilter);
        if (it != fRoomToPeripherals.end()) {
            periph.insert(periph.end(), it->second.begin(), it->second.end());
        }
    }
    return periph;
}

std::shared_ptr<BluetoothPeripheral> BluetoothSupervisor::getPeripheral(std::string const& bleAddress) {
    for (auto & [room, peripherals] : fRoomToPeripherals) {
        auto it = std::find_if(peripherals.begin(), peripherals.end(),
                    [&bleAddress] (std::shared_ptr<BluetoothPeripheral> & periph) {
                        return (periph->fAddress == bleAddress);
                    });
        if (it != peripherals.end()) {
            return *it;
        }
    }
    return nullptr;
}

void BluetoothSupervisor::init() {
    std::stringstream strOut;
    try {
        fpBleManager = BluetoothManager::get_bluetooth_manager();
    } catch(const std::runtime_error& e) {
        strOut << "[error] Domomo BluetoothSupervisor::init: error while initializing libtinyb "<< e.what();
        throw(strOut.str());
    }
    bool ret = fpBleManager->start_discovery();
    if (!ret) {
        strOut << "[error] Domomo BluetoothSupervisor::init: discovery not started.";
        throw(strOut.str());
    }
}

std::future<bool> BluetoothSupervisor::startScanAsync() {
    return std::async(&BluetoothSupervisor::asyncScan, this);
}

bool BluetoothSupervisor::asyncScan() {
    int counterMax = 0;
    std::cout << "[BluetoothSupervisor]: discovering devices... " << std::endl;
    std::vector<std::shared_ptr<BluetoothPeripheral>> allPeripherals = this->getPeripherals();
    do {
        auto list = this->fpBleManager->get_devices();
        for (auto it = list.begin(); it != list.end(); ++it) {
            for (auto itPeriph = allPeripherals.begin(); itPeriph != allPeripherals.end(); itPeriph++) {
                if ((*itPeriph)->fAddress == (*it)->get_address()) {
                    auto manufacturerData = (*it)->get_manufacturer_data();
                    if (manufacturerData.size() >= 1) {
                        auto const& data = manufacturerData.begin()->second;
                        if ((*itPeriph)->fType == BluetoothPeripheralType::SwitchBot) {
                            SwitchBotBluetoothPeripheral* pSwitchBot = dynamic_cast<SwitchBotBluetoothPeripheral*>((*itPeriph).get());
                            pSwitchBot->setData(data);
                        } else {
                            assert(false);
                        }
                    }
                    //remove from the list of objects to be "handled"
                    allPeripherals.erase(itPeriph);
                    break;
                }
            }
        }
        /* If not, wait and try again */
        std::this_thread::sleep_for(std::chrono::seconds(2));
    } while ((counterMax < 30) && !allPeripherals.empty());

    bool success = this->fpBleManager->stop_discovery();
    if (!success) {
        std::cerr << "[error] Domomo BluetoothSupervisor::scanThread: Fail to stop the BleManager.";
    }
    return success;
}



}
