# qt-openzwave
This is a [QT](https://www.qt.io) Wrapper for OpenZWave and contains ozwdaemon - a service that allows you to remotely manage a Z-Wave Network via [ozw-admin](https://github.com/OpenZWave/ozw-admin) or connect to a MQTT Broker.

## Home Assistant MQTT Client Adapter

<p align="center">
    <a href="http://bamboo.my-ho.st/bamboo/browse/OZW-OO/" alt="Build Status">
        <img src="http://bamboo.my-ho.st/bamboo/plugins/servlet/wittified/build-status/OZW-OO">
    </a>
</p>
  

A [Docker Container](https://hub.docker.com/r/openzwave/ozwdaemon) to connect to the [new Z-Wave Integration for Home Assistant - python-openzwave-mqtt](https://www.home-assistant.io/integrations/ozw/)

There are two types of Docker Containers published:
* A dedicated container that just contians ozwdaemon to bridge between the Z-Wave Network and a MQTT Broker
* A "All In One" container that also contains [ozw-admin](https://github.com/OpenZWave/ozw-admin) that exposes a VNC Port or a Web Based VNC Interface to manage your Z-Wave Network. 


Running the dedicated container (ozwdaemon):
-------------
Start a container with one of the following examples:

**Dedicated Docker example:**

```docker run -it --security-opt seccomp=unconfined --device=/dev/ttyUSB0 -v /tmp/ozw/config:/opt/ozw/config -e MQTT_SERVER="10.100.200.102" -e USB_PATH=/dev/ttyUSB0 openzwave/ozwdaemon:latest```

**Dedicated Docker-compose example:** 
``` yaml
version: '3'
services:
  qt-openzwave:
    image: openzwave/ozwdaemon:latest
    container_name: "qt-openzwave"
    security_opt:
      - seccomp:unconfined
    devices:
      - "/dev/ttyUSB0:/dev/ttyUSB0"
    volumes:
      - /tmp/ozw:/opt/ozw/config
    environment:
      MQTT_SERVER: "192.168.0.1"
      MQTT_USERNAME: "my-username"
      MQTT_PASSWORD: "my-password"
      USB_PATH: "/dev/ttyUSB0"
      OZW_NETWORK_KEY: "0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"
    restart: unless-stopped 
```

**All-In-One Docker example:**

```docker run -it --security-opt seccomp=unconfined --device=/dev/ttyUSB0 -v /tmp/ozw/config:/opt/ozw -e MQTT_SERVER="10.100.200.102" -e USB_PATH=/dev/ttyUSB0 openzwave/ozwdaemon:allinone-latest```

**All-In-One Docker-compose example:** 
``` yaml
version: '3'
services:
  qt-openzwave:
    image: openzwave/ozwdaemon:allinone-latest
    container_name: "qt-openzwave"
    security_opt:
      - seccomp:unconfined
    devices:
      - "/dev/ttyUSB0:/dev/ttyUSB0"
    volumes:
      - /tmp/ozw:/opt/ozw/config
    environment:
      MQTT_SERVER: "192.168.0.1"
      MQTT_USERNAME: "my-username"
      MQTT_PASSWORD: "my-password"
      USB_PATH: "/dev/ttyUSB0"
      OZW_NETWORK_KEY: "0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"
    restart: unless-stopped 
```

**Enviroment Variables**

* MQTT_SERVER - The IP Address of your MQTT Broker
* MQTT_USERNAME - The Username to connect to your MQTT Broker
* MQTT_PASSWORD - The Password used to connect ot your MQTT Broker
* USB_PATH - The Path to the USB Stick/Serial Device file in your container. You should also replace the --device parameter with the correct path to your USB Stick/Serial Device
* OZW_NETWORK_KEY - The Network Key to secure communications with your devices (that are included Securely) - DO NOT LOSE THIS KEY OTHERWISE YOU WILL HAVE TO REINCLUDE YOUR SECURED DEVICES
* OZW_INSTANCE - If you want to run more than one Z-Wave Network with multiple ozwdaemons, set this enviroment variable to a number greater than 1 (It affects the base topic that this instance will publish Messages to on your MQTT Broker - OpenZWave/#/...)
* OZW_CONFIG_DIR - Change the path inside the container that points to the Device Database. You should not need to modify this.
* OZW_USER_DIR - Change the path where Network Specific Cache/Config Files are stored. You should not need to modify this.
* STOP_ON_FAILURE - ozwdaemon will exit if it detects any failure, such as inability to connect to the MQTT Server, or open the Z-Wave Controller - Defaults to True.
* MQTT_TLS - If ozwdaemon should connect with TLS encryption to your MQTT Broker

For the All In One Image:

* VNC_PORT - The Port to run the VNC port for ozw-admin on. Defaults to 5901
* WEB_PORT - The Port to run the Integrated Webserver for a web based VNC Client (NoVNC). Defaults to 7800

the `--security-opt seccomp=unconfined` is needed to generate meaningfull backtraces, otherwise it will be difficult for us to debug.

** MQTT API Documentation: ** 

Please see [docs/MQTT.md](docs/MQTT.md) for complete instructions, including settting up Network Keys etc

## Building Instructions

The Main Requirements for building this tool is QT 5.12 (LTS) with the QTRemoteObjects Module enabled. Not All Distributions currently ship this version of QT, so manually installing/updated QT on those versions is required.

The Distributions known to meet the minimum requirements are:
* Debian Bullseye
* Debain Buster (but with Unstable Branch for QtRemoteObjects enabled)
* Fedora 30 and above
* Ubuntu 20.04 and above

Other Dependancies:
* [QtMQTT Module](https://github.com/qt/qtmqtt) - You should clone and checkout the branch that matches your QT version
* [open-zwave](https://github.com/OpenZWave/open-zwave) - You should clone open-zwave in the same top level as qt-openzwave

For Actual Build Instructions - Please consult the [docker/Dockerfile](docker/Dockerfile) - You can ignore the depot_tools (Google Breakpad) as this is used for Crash Reporting that should not be used for non-official builds

For All-In-One Container - Please also consult the ozw-admin repository for the requirements to build ozw-admin. 

## QT Wrapper Interface

Documentation is in progress. See the qt-simpleclient for a basic example. 
