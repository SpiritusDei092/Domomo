#include "MqttPublisher.hpp"

namespace mqtt {


const char* LWT_PAYLOAD = "<unwanted_mqtt_disconnect>";

const int QOS = 1;

const auto TIMEOUT = std::chrono::seconds(10);

MqttPublisher::MqttPublisher(std::string const& serverAddress, std::string const& clientId) {

    fMqttClient = std::make_shared<mqtt::async_client>(serverAddress, clientId);
    fpMqttCallback = std::make_shared<mqtt::mqtt_callback>(this);
    fMqttClient->set_callback(*fpMqttCallback);
}

bool MqttPublisher::connect(std::string const& topic) {
    try {
        auto connOpts = mqtt::connect_options_builder()
            .clean_session()
            .will(mqtt::message(topic, LWT_PAYLOAD, QOS))
            .finalize();

        fMqttConnectionStatus = ConnectionStatus::WaitingForConnection;
        addLog("MqttPublisher:connect");
        mqtt::token_ptr conntok = fMqttClient->connect(connOpts);
        conntok->wait();
        fConnectedTopic = topic;
        std::cout << "[MqttPublisher]: Mqtt connected. " << std::endl;
        return true;
    } catch (mqtt::exception const& exc) {
        std::string log = "MqttPublisher:connect:exception:";
        log.append(exc.what());
        addLog(log);
        return false;
    }
}

bool MqttPublisher::disconnect() {
    try {
        fMqttClient->disconnect()->wait();
        fConnectedTopic.clear();
        addLog("MqttPublisher:disconnect");
        return true;
    } catch (mqtt::exception const& exc) {
        std::string log = "MqttPublisher:disconnect:exception:";
        log.append(exc.what());
        addLog(log);
        return false;
    }
}

bool MqttPublisher::publish(std::string domoticzIdx, std::string svalue) {
    std::string payload = "{ \"idx\": " + domoticzIdx +
                                ", \"nvalue\": 0, " +
                                "\"svalue\": \"" + svalue + "\"}";
    try {
        mqtt::message_ptr pubmsg = mqtt::make_message(fConnectedTopic, payload);
        addLog("MqttPublisher:publish");
        pubmsg->set_qos(QOS);
        return fMqttClient->publish(pubmsg)->wait_for(TIMEOUT);
    } catch (mqtt::exception const& exc) {
        std::string log = "MqttPublisher:publish:exception:";
        log.append(exc.what());
        addLog(log);
        return false;
    }
}

}