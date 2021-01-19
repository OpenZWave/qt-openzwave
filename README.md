# qt-openzwave [![Build Status](http://bamboo.my-ho.st/bamboo/plugins/servlet/wittified/build-status/OZW-OO)](http://bamboo.my-ho.st/bamboo/browse/OZW-OO/)

This is a [QT](https://www.qt.io) Wrapper for OpenZWave and contains ozwdaemon - a service that allows you to remotely manage a Z-Wave Network via [ozw-admin](https://github.com/OpenZWave/ozw-admin) or connect to a MQTT Broker.

## Docker

The ozwdaemon application is published as a [Docker image](https://hub.docker.com/r/openzwave/ozwdaemon). There are two types of images:

* A dedicated container that contains only the ozwdaemon service
* A "All In One" container that includes both the ozwdaemon service and an embedded version of the [ozw-admin](https://github.com/OpenZWave/ozw-admin) GUI management tool.

### Using the standalone image

The dedicated ozwdaemon image contains only the ozwdaemon application and none of the management tools.

The ozwdaemon uses the `/opt/ozw/config` directory as the default location to store its cache files, device config files, logs and crashdumps. Be sure to use a bind mount or named volume that is persistent.

An example command for running the container would be:
```
docker run -it \
    --security-opt seccomp=unconfined \
    --device=/dev/ttyUSB0 \
    -v $PWD/ozw:/opt/ozw/config \
    -e MQTT_SERVER="10.100.200.102" \
    -e USB_PATH=/dev/ttyUSB0 \
    -p 1983:1983 \
    openzwave/ozwdaemon:latest
```

An example using Docker Compose would be this `docker-compose.yaml` file:
```yaml
version: '3'
services:
  ozwd:
    image: openzwave/ozwdaemon:latest
    container_name: "ozwd"
    security_opt:
      - seccomp:unconfined
    devices:
      - "/dev/ttyUSB0"
    volumes:
      - ./ozw:/opt/ozw/config
    ports:
      - "1983:1983"
    environment:
      MQTT_SERVER: "192.168.0.1"
      MQTT_USERNAME: "my-username"
      MQTT_PASSWORD: "my-password"
      USB_PATH: "/dev/ttyUSB0"
      OZW_NETWORK_KEY: "0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"
    restart: unless-stopped
```

The `--security-opt seccomp=unconfined` is needed to generate meaningful backtraces and crashdump files, otherwise it will be difficult for us to debug.

ozwdaemon will shutdown in certain conditions (errors, unable to connect to the MQTT broker), which will exit the container. Be sure to set an appropriate restart policy if you want the container to restart on failure.

#### Enviroment Variables

The container is configurable via several environment variables.

* MQTT_SERVER - The IP address or hostname of the MQTT broker. Defaults to `localhost`.
* MQTT_PORT - The port number of the MQTT broker. Defaults to `1883`.
* MQTT_USERNAME - The username to use when connecting to the MQTT broker. Do not set for anonymous logins.
* MQTT_PASSWORD - The password used to connect to the MQTT broker, if needed.
* MQTT_CONNECT_TIMEOUT - The number of seconds to wait for the MQTT broker to become available before starting ozwdaemon. If a connection cannot be made before the timeout expires the container will exit. Defaults to 30 seconds.
* USB_PATH - The pathname of the USB stick/serial device file in the container. This value must match the name of the device that was mapped from the host with the Docker `--device` option. Defaults to `/dev/ttyUSB0`.
* OZW_NETWORK_KEY - The Network Key to secure communications with your devices (that are included Securely) - DO NOT LOSE THIS KEY OTHERWISE YOU WILL HAVE TO REINCLUDE YOUR SECURED DEVICES. Defaults to no network key (secure inclusion not possible). Alternatively, for increased security, a Docker secret named OZW_Network_Key can be supplied instead.
* OZW_INSTANCE - Multiple Z-Wave networks can run concurrently by starting an individual container for each network. To distinguish the networks, set this enviroment variable to a unique value for each container instance. This affects the base topic that is published to the MQTT broker - `OpenZWave/<OZW_INSTANCE>/#`. Defaults to `1`.
* OZW_CONFIG_DIR - Set the path inside the container that points to the Device Database. Most users should not need to modify this. Defaults to `/opt/ozw/config`.
* OZW_USER_DIR - Change the path where Network Specific Cache/Config Files are stored. Most users should not need to modify this. Defaults to `/opt/ozw/config`.
* OZW_AUTH_KEY - Remote management (ozw-admin) authorization key. Alternatively, for increased security, a Docker secret named OZW_Auth_Key can be supplied instead.
* STOP_ON_FAILURE - If true, ozwdaemon will exit when it detects any failure, such as the inability to connect to the MQTT broker, or open the Z-Wave Controller. Valid values are `true` or `false`. Defaults to `true`.
* MQTT_TLS - If true, ozwdaemon will connect with TLS encryption to the MQTT broker. Valid values are `true` or `false`. Defaults to `false`.

#### Exposed Ports

The standalone image exposes the following ports:

* 1983 - ozw-admin port

#### Logs

All log messages are printed to the container's console (stdout). They can be viewed with the docker logs command, e.g. `docker logs <container id>`.

### Using the All-In-One Image

The All-In-One image is a variant of the standalone image that includes both ozwdaemon and embedded version of the ozw-admin GUI management tool. The embedded ozw-admin tool is accessible via VNC, either with an external VNC client or the hosted HTML VNC client. The desktop application is not necessary in this case, although it can also be used.

This image uses the same `/opt/ozw/config` directory to store persistent data.

An example command for running the container would be:
```
docker run -it \
    --security-opt seccomp=unconfined \
    --device=/dev/ttyUSB0 \
    -v $PWD/ozw:/opt/ozw/config \
    -e MQTT_SERVER="10.100.200.102" \
    -e USB_PATH=/dev/ttyUSB0 \
    -p 1983:1983 \
    -p 5901:5901 \
    -p 7800:7800 \
    openzwave/ozwdaemon:allinone-latest
```

An example using Docker Compose would be this `docker-compose.yaml` file:
``` yaml
version: '3'
services:
  ozwd:
    image: openzwave/ozwdaemon:allinone-latest
    container_name: "ozwd"
    security_opt:
      - seccomp:unconfined
    devices:
      - "/dev/ttyUSB0"
    volumes:
      - ./ozw:/opt/ozw/config
    ports:
      - "1983:1983"
      - "5901:5901"
      - "7800:7800"
    environment:
      MQTT_SERVER: "192.168.0.1"
      MQTT_USERNAME: "my-username"
      MQTT_PASSWORD: "my-password"
      USB_PATH: "/dev/ttyUSB0"
      OZW_NETWORK_KEY: "0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"
    restart: unless-stopped
```

The `--security-opt seccomp=unconfined` is needed to generate meaningful backtraces and crashdump files, otherwise it will be difficult for us to debug.

The ozwdaemon and ozw-admin processes are managed by a supervisor watchdog. The watchdog will restart the containers in certain error conditions. It is also advisable to set an appropriate restart policy in case the supervisor exits.

#### Enviroment Variables

All-In-One image supports all of the same environment variables as the standalone image. In addition, the following settings are available:

* VNC_PORT - The VNC server port number. Provides remote access to the ozw-admin application via VNC. Defaults to `5901`.
* WEB_PORT - The integrated HTTP server port number. Provides a web based VNC client (NoVNC). Defaults to `7800`.

#### Exposed Ports

The All-In-One image exposes the following ports:

* 1983 - ozw-admin port
* 5901 - VNC server port
* 7800 - HTML VNC client port

#### Logs

Each application saves its own logs into separate log files. By default the logs are saved in the `/opt/ozw/config/logs` directory. Using the examples above, the logs would be accessible in the `$PWD/ozw/logs` directory.

### Managing the Z-Wave Network

Each of the image variants provides one or more ways of remotely managing the Z-Wave network.

1. The standalone and All-In-One images expose the ozw-admin remote access port. The desktop application version of ozw-admin GUI tool can be installed on a PC and can remotely connect to the running container. The default ozw-admin port number is 1983. In the Remote OZWDaemon section of the Open dialog, specify the container host IP address and port 1983 to make a remote connection.
2. The All-In-One image exposes a VNC server which is running an embedded version of the ozw-admin GUI tool. A dedicated VNC client can connect to the host IP address and the configured VNC port (default 5901) and the ozw-admin tool will be available remotely.
3. The All-In-One image exposes a Web Based VNC client which connects to the internal VNC server that is running the ozw-admin GUI tool. Use your web browser and connect to the IP address of the container host and the configured HTML VNC client port (default 7800), for example http://192.168.1.2:7800. This will open a Web Based VNC session where you can control ozw-admin.

For the VNC remote access methods, use the Open button to open the connection dialog. Click on the Start button in the Remote OZWDaemon section. Leave the host as localhost. Leave the port as 1983 or change if the ozw-admin port number was customized.

## MQTT API Documentation

Please see [docs/MQTT.md](docs/MQTT.md) for complete instructions, including settting up Network Keys, etc.

## Building Instructions

The Main Requirements for building this tool is QT 5.12 (LTS) with the QTRemoteObjects Module enabled. Not All Distributions currently ship this version of QT, so manually installing/updated QT on those versions is required.

You can use [ozw-base](https://hub.docker.com/repository/docker/fishwaldo/ozw-base) as a stable image based on Buster with a custom build of QT 5.12.x

Other Dependancies:
* [QtMQTT Module](https://github.com/qt/qtmqtt) - You should clone and checkout the branch that matches your QT version
* [open-zwave](https://github.com/OpenZWave/open-zwave) - You should clone open-zwave in the same top level as qt-openzwave

For Building On Docker, you must use the enable [Docker BuildKit](https://docs.docker.com/develop/develop-images/build_enhancements/) and the command to build the base image would be:

```
DOCKER_BUILDKIT=1 docker build . -f Docker/Dockerfile -t test --build-arg distcchosts='<list of distcc hosts>' --target base
```

or for the AllInOne Image:
```
DOCKER_BUILDKIT=1 docker build . -f Docker/Dockerfile -t test --build-arg distcchosts='<list of distcc hosts>' --target allinone
```

You can omit the --build-arg distcchosts=<..> portion if you do not have distcc configured on your network

For Building stand alone binaries - Please consult the [docker/Dockerfile](Docker/Dockerfile) - You can ignore the depot_tools (Google Breakpad) as this is used for Crash Reporting that should not be used for non-official builds

## Home Assistant Integration

ozwdaemon integrates directly with Home Assistant. See the [OpenZWave (Beta)](https://www.home-assistant.io/integrations/ozw/) integration documentationi for more information.

## QT Wrapper Interface

Documentation is in progress. See the qt-simpleclient for a basic example.
