# qt-openzwave
This is a [QT](https://www.qt.io) Wrapper for OpenZWave and contains ozwdaemon - a service that allows you to remotely manage a Z-Wave Network via [ozw-admin](https://github.com/OpenZWave/ozw-admin) or connect to a MQTT Broker.

## Home Assistant MQTT Client Adapter

<p align="center">
    <a href="http://bamboo.my-ho.st/bamboo/browse/OZW-OO/" alt="Build Status">
        <img src="http://bamboo.my-ho.st/bamboo/plugins/servlet/wittified/build-status/OZW-OO">
    </a>
</p>
  

A [Docker Container](https://hub.docker.com/r/openzwave/ozwdaemon) to connect to the [new Z-Wave Integration for Home Assistant OpenZWave (beta)](https://www.home-assistant.io/integrations/ozw/)

Running:
-------------
Start a container with one of the following examples:

**Docker example:**

```docker run -it --security-opt seccomp=unconfined --device=/dev/ttyUSB0 -v /tmp/ozw/config:/opt/ozw -e MQTT_SERVER="10.100.200.102" -e USB_PATH=/dev/ttyUSB0 openzwave/ozwdaemon:latest```

**Docker-compose example:** 
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
* replace MQTT_SERVER with the IP address of the MQTT Server
* replace MQTT_USERNAME and MQTT_PASSWORD with authentication details from the MQTT Server (remove if not enabled)
* replace all /dev/ttyUSB0 entries with the path to your USB Stick.
* replace OZW_NETWORK_KEY with the network key of your Z-wave network (remove if not enabled)
* OpenZWave Config Files are copied on startup to the host directory specified by /tmp/ozw - You should replace it with a directory that is persistent.

the `--security-opt seccomp=unconfined` is needed to generate meaningfull backtraces, otherwise it will be difficult for us to debug.

Please see [docs/MQTT.md](docs/MQTT.md) for complete instructions, including settting up Network Keys etc

## QT Wrapper Interface

Documentation is in progress. See the qt-simpleclient for a basic example. 
