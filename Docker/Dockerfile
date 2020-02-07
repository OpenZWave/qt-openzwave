############################################################
# Dockerfile to build OpenZWave Library container images
# Based on CentOS7
############################################################

# Set the base image to Alpine
FROM debian:buster-slim as builder

# File Author / Maintainer
LABEL maintainer="justin@dynam.ac"

WORKDIR /opt

RUN echo 'APT::Default-Release "stable";' > /etc/apt/apt.conf.d/99defaultrelease && echo 'deb     http://ftp.debian.org/debian/    unstable main contrib non-free' > /etc/apt/sources.list.d/unstable.list && echo 'deb     http://ftp.debian.org/debian/    experimental main contrib non-free' >> /etc/apt/sources.list.d/unstable.list && apt update
RUN apt-get -y install rapidjson-dev git g++ cmake make pkgconf bash python wget joe mc libunwind-dev libcurl4-openssl-dev && apt-get -y -t unstable install qt5-default libqt5websockets5-dev && apt-get -y -t unstable install qtbase5-private-dev && apt-get -y -t unstable install g++ && apt-get -y -t experimental install libqt5remoteobjects5-dev libqt5remoteobjects5-bin 
ENV PATH=$PATH:/opt/depot_tools/
RUN git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git && mkdir breakpad && cd breakpad && fetch breakpad && cd src && ./configure --disable-processor && make && make install
RUN git clone https://github.com/qt/qtmqtt.git && cd qtmqtt && git checkout 5.12 && /usr/lib/qt5/bin/qmake QT_BUILD_PARTS="libs tools" && make && make install
RUN git clone https://github.com/OpenZWave/open-zwave.git && cd open-zwave && make -j4 && make install
COPY . /opt/qt-openzwave/
RUN cd qt-openzwave && if [ -f Makefile ]; then /usr/lib/qt5/bin/qmake -r; make distclean; fi && /usr/lib/qt5/bin/qmake -r "CONFIG+=BreakPad" &&  make -j4 && make install

FROM debian:buster-slim

LABEL maintainer="justin@dynam.ac"

WORKDIR /opt

RUN echo 'APT::Default-Release "stable";' > /etc/apt/apt.conf.d/99defaultrelease \
    && echo 'deb http://ftp.debian.org/debian/ unstable main contrib non-free' > /etc/apt/sources.list.d/unstable.list \
    && echo 'deb http://ftp.debian.org/debian/ experimental main contrib non-free' >> /etc/apt/sources.list.d/unstable.list \
    && apt update \
    && apt-get -y install libunwind8 libcurl4 binutils \
    && apt-get -y -t unstable install libqt5core5a libqt5network5 libqt5websockets5 \
    && apt-get -y -t experimental install libqt5remoteobjects5 \
    && strip --remove-section=.note.ABI-tag /usr/lib/x86_64-linux-gnu/libQt5Core.so.5 \
    && apt-get -y purge binutils \
    && apt-get -y autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir -p /opt/ozw/config/crashes/

COPY --from=builder /usr/local/bin/* /usr/local/bin/
COPY --from=builder /usr/lib/*/libqt-* /usr/local/lib/
COPY --from=builder /usr/local/lib*/libopenzwave* /usr/local/lib/
COPY --from=builder /usr/lib/*/libQt5Mqtt* /usr/local/lib/
COPY --from=builder /opt/qt-openzwave/tools/* /usr/local/bin/
COPY --from=builder /usr/share/qt5/qt-openzwavedatabase.rcc /usr/share/qt5/
RUN mkdir -p /opt/ozw/config/crashes/
ENV LD_LIBRARY_PATH="/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH"
ENV USB_PATH="/dev/ttyUSB0"
ENV MQTT_SERVER="localhost"
ENV MQTT_PORT="1883"
ENV OZW_INSTANCE="1"
ENV BP_DB_PATH="/opt/ozw/config/crashes/"
WORKDIR /opt/ozw/
EXPOSE 1983
VOLUME ["/opt/ozw/config/"]
ENTRYPOINT /usr/local/bin/ozwdaemon -s $USB_PATH -c /opt/ozw/config/ -u /opt/ozw/config/ --mqtt-server $MQTT_SERVER --mqtt-port $MQTT_PORT --stop-on-failure --mqtt-instance $OZW_INSTANCE
