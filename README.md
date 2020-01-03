# qt-openzwave
This is a [QT](https://www.qt.io) Wrapper for OpenZWave and contains ozwdaemon - a service that allows you to remotely manage a Z-Wave Network via [ozw-admin](https://github.com/OpenZWave/ozw-admin) or connect to a MQTT Broker.

## Home Assistant MQTT Client Adapter

A [Docker Container](https://hub.docker.com/r/openzwave/ozwdaemon) to connect to the [new Z-Wave Integration for Home Assistant - python-openzwave-mqtt](https://github.com/cgarwood/python-openzwave-mqtt)

Running:
-------------
Start a container with the following command line:

```docker run -it --device=/dev/ttyUSB0 -v /tmp/ozw:/opt/ozw/config -e MQTT_SERVER="10.100.200.102" -e USBPATH=/dev/ttyUSB0 openzwave/ozwdaemon:latest```

* replace MQTT_SERVER with the IP address of the MQTT Server 
* replace all /dev/ttyUSB0 entries with the path to your USB Stick.
* OpenZWave Config Files are copied on startup to the host directory specified by /tmp/ozw - You should replace it with a directory that is persistent. 

Please see [docs/MQTT.md](docs/MQTT.md) for further information

## QT Wrapper Interface

Documentation is in progress. See the qt-simpleclient for a basic example. 
