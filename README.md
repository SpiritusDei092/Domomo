# Domomo

Domomo is a lightweight gateway that reads Bluetooth Low Energy (BLE) frames from SwitchBot temperature/humidity sensors and publishes the data to an MQTT broker. This allows easy integration with home automation platforms such as Domoticz.

## Features

- Scans for SwitchBot BLE temperature/humidity sensors.
- Publishes sensor data to MQTT topics.
- Designed for easy scheduling (e.g., using cron) to poll sensors at regular intervals.
- Flexible configuration via `home_definition.json`.

---

## Requirements

- **MQTT Libraries**:  
  - [Eclipse Paho MQTT C](https://github.com/eclipse/paho.mqtt.c)  
  - [Eclipse Paho MQTT C++](https://github.com/eclipse/paho.mqtt.cpp)
- **Bluetooth Library**:  
  - [Intel TinyB](https://github.com/intel-iot-devkit/tinyb) (install in `/usr`)
- **Boost C++ Libraries**:  
  - `sudo apt-get install libboost-all-dev`
- **MQTT Broker**:  
  - [Mosquitto](https://mosquitto.org/) and `mosquitto-clients`
- **CMake**: for building the project
- **A compatible Linux device** (e.g., Raspberry Pi) with BLE support

---

## Installation

1. **Install Dependencies**

   ```sh
   sudo apt-get update
   sudo apt-get install libboost-all-dev mosquitto mosquitto-clients cmake g++
   # Install Paho MQTT C and C++ libraries per their instructions
   # Install TinyB per its instructions
   ```

2. **Clone the Repository**

   ```sh
   git clone https://github.com/SpiritusDei092/Domomo.git
   cd Domomo
   ```

3. **Build the Project**

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

---

## Configuration

Before running, configure your sensors and MQTT settings in `home_definition.json`:

```json
{
  "mqtt": {
    "serverAddress": "127.0.0.1",
    "clientId": "raspberryPi1"
  },
  "rooms": [
    "salon",
    "jardin"
  ],
  "periphs": [
    {
      "addr": "CF:B3:6E:3F:E8:23",
      "room": "salon",
      "idxdomoticz": 1,
      "name": "Température/Humidité",
      "mqttFormat": "%d;%u;1",
      "type": "SwitchBot"
    },
    {
      "addr": "D6:34:C5:86:40:81",
      "room": "jardin",
      "idxdomoticz": 2,
      "name": "Température/Humidité",
      "mqttFormat": "%d;%u;1",
      "type": "SwitchBot"
    }
  ]
}
```

- Edit the `mqtt` section for your broker settings.
- List your sensor devices under `periphs`.

---

## Usage

1. **Run the Program**

   ```sh
   ./Domomo
   ```

2. **Automate with Cron**

   To poll sensors every 5 minutes, add to your crontab:

   ```sh
   */5 * * * * /path/to/Domomo
   ```

---

## How It Works

- The program scans for BLE advertising packets from configured SwitchBot sensors.
- Extracted data (temperature, humidity, etc.) is formatted and published to the configured MQTT broker.
- Home automation systems (such as Domoticz) can then consume the MQTT data.

---

## License

[MIT](LICENSE) (or specify your license here)

---

## Credits

- [Eclipse Paho MQTT](https://github.com/eclipse/paho.mqtt.c)
- [Eclipse Paho MQTT C++](https://github.com/eclipse/paho.mqtt.cpp)
- [Intel TinyB](https://github.com/intel-iot-devkit/tinyb)
- [Mosquitto](https://mosquitto.org/)