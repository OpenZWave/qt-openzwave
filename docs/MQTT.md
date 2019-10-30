MQTT Client for OpenZWave
=========================

Docker Repo: https://cloud.docker.com/u/openzwave/repository/docker/openzwave/ozw-mqtt

Copy "config" folder from OZW to /tmp/ozw/

Start Container with the following command line:
>docker run -it --device=/dev/ttyUSB0 -v /tmp/ozw:/opt/ozw/config -e MQTT_SERVER="10.100.200.102" -e USBPATH=/dev/ttyUSB0 ozw-mqtt:latest

(replace MQTT_SERVER with IP address of the MQTT Server and all /dev/ttyUSB0 entries with path to your USB Stick
