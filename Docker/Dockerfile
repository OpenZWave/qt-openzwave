FROM fishwaldo/qt-staticbuilds:5.12.4 as builder

MAINTAINER Justin Hammond

RUN dnf -y update \
 && dnf install -y rapidjson-devel \
 && dnf clean all

WORKDIR /src
RUN git clone https://github.com/qt/qtmqtt.git && cd qtmqtt && git checkout 5.12 && /opt/Qt/5.12.4/bin/qmake && make && make install
RUN git clone https://github.com/OpenZWave/open-zwave.git && cd open-zwave && make -j4 && make install
RUN git clone https://github.com/OpenZWave/qt-openzwave.git && cd qt-openzwave && git checkout mqtt && /opt/Qt/5.12.4/bin/qmake && make -j4 && make install

FROM fedora:25

RUN dnf -y update \
 && dnf install -y libstdc++ \
 && dnf clean all

COPY --from=builder /ozwdaemon /ozwdaemon
COPY --from=builder /usr/local/lib64/libopenzwave* /usr/lib64/

ENV USBPATH="/dev/ttyUSB0"
ENV MQTT_SERVER="localhost"
ENV MQTT_PORT="1883"
WORKDIR /opt/ozw/
EXPOSE 1983
VOLUME ["/opt/ozw/config/"]
ENTRYPOINT /usr/bin/catchsegv /ozwdaemon/bin/ozwdaemon -s $USBPATH -c /opt/ozw/config/ -u /opt/ozw/config/ --mqtt-server $MQTT_SERVER --mqtt-port $MQTT_PORT
