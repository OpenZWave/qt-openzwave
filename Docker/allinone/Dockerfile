############################################################
# Dockerfile to build OpenZWave Library container images
# Based on Debian
############################################################

# Set the base image to Debian
FROM debian:bullseye-slim as builder

# File Author / Maintainer
LABEL maintainer="justin@dynam.ac"

WORKDIR /opt

RUN apt update \
	&& apt-get -y install rapidjson-dev git cmake make pkgconf bash python wget joe mc libunwind-dev libcurl4-openssl-dev g++ qt5-default libqt5websockets5-dev qtbase5-private-dev libqt5remoteobjects5-dev libqt5remoteobjects5-bin libqt5svg5-dev libqt5serialport5-dev

ENV PATH=$PATH:/opt/depot_tools/
ENV DEPOT_TOOLS_UPDATE=0
RUN git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git \
	&& cd depot_tools && git checkout 464e9ff && cd .. \
	&& mkdir breakpad \
	&& cd breakpad \
	&& fetch breakpad \
	&& cd src \
	&& ./configure --disable-processor \
	&& make -j$(nproc)\
	&& make install
RUN git clone https://github.com/qt/qtmqtt.git \
	&& cd qtmqtt \
	&& git checkout 5.12.5 \
	&& /usr/lib/qt5/bin/qmake CONFIG+=release QT_BUILD_PARTS="libs tools" \
	&&  make -j$(nproc) \
	&& make install
RUN git clone https://github.com/OpenZWave/open-zwave.git \
	&& cd open-zwave \
	&& /bin/bash -c " make -j$(nproc)" \
	&& /bin/bash -c "make install"
	
ARG BP_CLIENTID
ARG BP_CLIENTKEY
ARG BUILDNUMBER=0
#RUN git clone https://github.com/OpenZWave/qt-openzwave.git \
COPY . /opt/qt-openzwave/
RUN	cd qt-openzwave \
	&& if [ -f Makefile ]; then /usr/lib/qt5/bin/qmake -r; make distclean; fi \
	&& /usr/lib/qt5/bin/qmake -r CONFIG+=release "BP_CLIENTID=$BP_CLIENTID" "BP_CLIENTKEY=$BP_CLIENTKEY" "BUILDNUMBER=$BUILDNUMBER" \
	&& make -j$(nproc) \
	&& make install

RUN git clone https://github.com/OpenZWave/ozw-admin.git \
	&& cd ozw-admin \
#	&& git checkout $(git describe --tags) \
	&& if [ -f Makefile ]; then /usr/lib/qt5/bin/qmake -r; make distclean; fi \
	&& /usr/lib/qt5/bin/qmake -r CONFIG+=release \
	&& make -j$(nproc) \
	&& make install

FROM debian:bullseye-slim

LABEL maintainer="justin@dynam.ac"

WORKDIR /opt

RUN apt update \
    	&& apt-get --no-install-recommends -y install libunwind8 libcurl4 binutils libqt5core5a libqt5network5 libqt5websockets5 libqt5remoteobjects5 libqt5svg5 novnc supervisor libqt5xml5 libqt5serialport5\
    	&& apt-get -y upgrade \
    	&& strip --remove-section=.note.ABI-tag /usr/lib/*/libQt5Core.so.5 \
    	&& apt-get -y purge binutils \
    	&& apt-get -y autoremove \
    	&& apt-get clean \
	&& mkdir -p /opt/novnc/app \
	&& mkdir -p /opt/novnc/core \
	&& mkdir -p /opt/novnc/vendor \
	&& cp -r /usr/share/novnc/app/* /opt/novnc/app/ \
	&& cp -r /usr/share/novnc/core/* /opt/novnc/core/ \
	&& cp -r /usr/share/novnc/vendor/* /opt/novnc/vendor/ \
	&& cp -r /usr/share/novnc/vnc*.html /opt/novnc/ \
	&& mkdir -p /opt/ozw/config/crashes/ \
	&& mkdir -p /opt/ozw/config/logs/ \
    	&& apt-get clean 

COPY --from=builder /usr/local/bin/* /usr/local/bin/
COPY --from=builder /usr/lib/*/libqt-* /usr/local/lib/
COPY --from=builder /usr/local/lib*/libopenzwave* /usr/local/lib/
COPY --from=builder /usr/lib/*/libQt5Mqtt* /usr/local/lib/
COPY --from=builder /opt/qt-openzwave/tools/* /usr/local/bin/
COPY --from=builder /usr/share/qt5/qt-openzwavedatabase.rcc /usr/share/OpenZWave/

COPY Docker/allinone/supervisord.conf /etc/
COPY Docker/allinone/novnc/* /opt/novnc/
COPY Docker/allinone/supervisord-watchdog /opt/
COPY Docker/allinone/entrypoint.sh /opt/
COPY Docker/startozwdaemon.sh /opt/

ENV LD_LIBRARY_PATH="/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH"
ENV USB_PATH="/dev/ttyUSB0"
ENV MQTT_SERVER="localhost"
ENV MQTT_PORT="1883"
ENV OZW_INSTANCE="1"
ENV BP_DB_PATH="/opt/ozw/config/crashes/"
ENV VNC_PORT=5901
ENV WEB_PORT=7800
ENV QT_ASSUME_STDERR_HAS_CONSOLE=1
WORKDIR /opt/ozw/
EXPOSE 1983
EXPOSE 5901
EXPOSE 7800
VOLUME ["/opt/ozw/config/"]
ENTRYPOINT ["/opt/entrypoint.sh"]
