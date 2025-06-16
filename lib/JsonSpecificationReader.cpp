#include "JsonSpecificationReader.hpp"
#include "global_ble/SwitchBotBluetoothPeripheral.hpp"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>


constexpr std::string_view periphs{"periphs"};
constexpr std::string_view periphs_addr{"addr"};
constexpr std::string_view periphs_room{"room"};
constexpr std::string_view periphs_name{"name"};
constexpr std::string_view periphs_domoticzidx{"idxdomoticz"};
constexpr std::string_view periphs_mqttformat{"mqttFormat"};
constexpr std::string_view periphs_type{"type"};

JsonSpecificationReader::JsonSpecificationReader(std::filesystem::path const& jsonDefinitionFile) {
    if (!std::filesystem::exists(jsonDefinitionFile)) {
        std::stringstream strOut;
        strOut << "[error] Domomo JsonSpecificationReader::readJsonSpecification: File \""
               << jsonDefinitionFile.string() << "\" not found.";
        throw(strOut.str());
    }
    readJsonSpecification(jsonDefinitionFile);
}

void JsonSpecificationReader::readJsonSpecification(std::filesystem::path const& jsonDefinitionFile) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(jsonDefinitionFile.string(), pt);

    // read the mqtt
    try {
        auto const& mqttNode = pt.get_child("mqtt");
        fMqttServer = mqttNode.get_child("serverAddress").data();
        fMqttClientId = mqttNode.get_child("clientId").data();
    } catch (...) {
        throw "Mqtt information not found \n";
    }

    // read the defined rooms
    for (boost::property_tree::ptree::value_type &roomNode : pt.get_child("rooms")) {
        std::string roomName = roomNode.second.data();
        rooms.push_back(roomName);
        fRoomToPeripherals.emplace(roomName, std::vector<std::shared_ptr<global_ble::BluetoothPeripheral>>());
    }

    // loop on "periph"
    for (boost::property_tree::ptree::value_type &periph : pt.get_child(periphs.data())) {
        // read address
        auto itAddrNode = periph.second.find(periphs_addr.data());
        if (itAddrNode == periph.second.not_found()) {
            std::cerr << "[error] Domomo BluetoothSupervisor::readJsonSpecification: "<< periphs_addr.data() << " field not found for periph definition\n";
        }
        std::string address = itAddrNode->second.data();

        // read room
        auto itRoomNode = periph.second.find(periphs_room.data());
        if (itRoomNode == periph.second.not_found()) {
            std::cerr << "[error] Domomo BluetoothSupervisor::readJsonSpecification:: "<< periphs_room.data() << " field not found for periph definition\n";
        }
        std::string room = itRoomNode->second.data();

        // read name
        auto itNameNode = periph.second.find(periphs_name.data());
        if (itNameNode == periph.second.not_found()) {
            std::cerr << "[error] Domomo BluetoothSupervisor::readJsonSpecification:: "<< periphs_name.data() << " field not found for periph definition\n";
        }
        std::string name = itNameNode->second.data();

        // read type
        auto itPeriphNode = periph.second.find(periphs_type.data());
        if (itPeriphNode == periph.second.not_found()) {
            std::cerr << "[error] Domomo BluetoothSupervisor::readJsonSpecification:: "<< periphs_type.data() << " field not found for periph definition\n";
        }
        std::string type = itPeriphNode->second.data();

        // domoticz idx
        auto itDomoticzIdx = periph.second.find(periphs_domoticzidx.data());
        if (itDomoticzIdx == periph.second.not_found()) {
            std::cerr << "[error] Domomo BluetoothSupervisor::readJsonSpecification:: "<< periphs_domoticzidx.data() << " field not found for periph definition\n";
        }
        int idx = itDomoticzIdx->second.get_value<int>();

        // read type
        auto itMqttFormat = periph.second.find(periphs_mqttformat.data());
        if (itMqttFormat == periph.second.not_found()) {
            std::cerr << "[error] Domomo BluetoothSupervisor::readJsonSpecification:: "<< periphs_mqttformat.data() << " field not found for periph definition\n";
        }
        std::string mqttFormat = itMqttFormat->second.data();


        #ifdef VERBOSE_DEBUG
        std::cout << " address:" << address << " - room:" << room << " - name:"<< name << " - idx domoticz" << idx << '\n';
        #endif
        auto it = fRoomToPeripherals.find(room);
        if (it == fRoomToPeripherals.end()) {
            std::cerr << "[error] Domomo BluetoothSupervisor::readJsonSpecification:: peripheral room \""<< room << "\" field not found in room list\n";
        } else {
            if (type == SwitchBotBluetoothPeripheral::fTypeName) {
                it->second.push_back(std::make_shared<SwitchBotBluetoothPeripheral>(room, name, address, idx, mqttFormat));
            }
        }
    }

#ifdef VERBOSE_DEBUG
    for(auto const& room : rooms) {
        std::cout << room << '\n';
    }
    for (auto const& [room, periphs] : fRoomToPeripherals) {
        for (auto const& peripheral : periphs) {
            std::cout << peripheral.toString() << '\n';
        }
    }
#endif
}
