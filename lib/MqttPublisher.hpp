#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <vector>
#include "mqtt/async_client.h"
#include "spec.hpp"

namespace mqtt {

class LIBFOO_SHLIB_EXPORTED BaseMqttPubliher {
public:
    enum class ConnectionStatus {
        NotConnected = 0,
        WaitingForConnection,
        Connected,
        ConnectionLost
    };
    virtual void addLog(std::string const& log) = 0;
    virtual void setStatus(ConnectionStatus st) = 0;
};

class LIBFOO_SHLIB_EXPORTED mqtt_callback : public virtual mqtt::callback {
public:
    mqtt_callback(BaseMqttPubliher* pMqttPusblisher): fpMqttPublisher(pMqttPusblisher) {}
    void connected(string const&) {
        fpMqttPublisher->setStatus(BaseMqttPubliher::ConnectionStatus::Connected);
        std::string log = "mqtt_callback:connected";
        fpMqttPublisher->addLog(log);
    }

    void connection_lost(std::string const& cause) override {
        fpMqttPublisher->setStatus(BaseMqttPubliher::ConnectionStatus::ConnectionLost);
        std::string log = "mqtt_callback:connection_lost:";
        if (!cause.empty()) {
            log.append(cause);
        } else {
            log.append("cause unknown");
        }
        fpMqttPublisher->addLog(log);
    }

    void delivery_complete(mqtt::delivery_token_ptr tok) override {
        std::string log = "mqtt_callback:delivery_complete:token:";
        log.append(std::to_string(tok ? tok->get_message_id() : -1));
        fpMqttPublisher->addLog(log);
    }
private:
    BaseMqttPubliher* fpMqttPublisher;
};

class LIBFOO_SHLIB_EXPORTED MqttPublisher: public BaseMqttPubliher {
public:
    MqttPublisher() = delete;
    MqttPublisher(std::string const& serverAddress, std::string const& clientId);

	bool connect(std::string const& topic);
    bool disconnect();
    bool publish(std::string domoticzIdx, std::string svalue);
    bool isConnected() {
        return fMqttConnectionStatus == ConnectionStatus::Connected;
    }
    void activateLog() { fLogActivated = true; }
    void printLog() const {
        for (auto const& log : fLogs) {
            std::cout << log << "\n";
        }
    }
private:
    void setStatus(ConnectionStatus st) override {
        fMqttConnectionStatus = st;
    }
    void addLog(std::string const& log) override {
        if (fLogActivated) {
            // add time point to the log
            const std::time_t utc = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::string timeString(100,'\0');
            // Monday 12 March 2023 - 04:37:12 ||---> la phrase du log
            std::strftime(timeString.data(), 100, "%A %d %B %Y - %T", std::localtime(&utc));
            fLogs.push_back(timeString + "||---> " + log);
        }
    }
    std::vector<std::string> fLogs;
    ConnectionStatus fMqttConnectionStatus = ConnectionStatus::NotConnected;
    mqtt::async_client::ptr_t fMqttClient;
    std::shared_ptr<mqtt::mqtt_callback> fpMqttCallback;
    std::string fConnectedTopic;
    bool fLogActivated = false;
};

}