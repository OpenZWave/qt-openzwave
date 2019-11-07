############################################################
# Dockerfile to build OpenZWave Library container images
# Based on CentOS7
############################################################

# Set the base image to Ubuntu
FROM fishwaldo/qt-staticbuilds:5.12.4 as builder

# File Author / Maintainer
MAINTAINER Justin Hammond

WORKDIR /opt

RUN git clone https://github.com/qt/qtmqtt.git && cd qtmqtt && git checkout 5.12 && /opt/Qt/5.12.4/bin/qmake && make && make install
RUN git clone https://github.com/OpenZWave/open-zwave.git && cd open-zwave && make -j4 && make install
RUN git clone https://github.com/OpenZWave/qt-openzwave.git && cd qt-openzwave && git checkout mqtt && /opt/Qt/5.12.4/bin/qmake && make -j4 && make install
#RUN ls -lah /usr/local/lib64/
#RUN ls /opt/
#RUN ls -lah /ozwdaemon/bin/
#RUN LD_LIBRARY_PATH="/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH" /ozwdaemon/bin/ozwdaemon
RUN mkdir /opt/ozw/ && mkdir /opt/ozw/config/

ENV LD_LIBRARY_PATH="/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH"
ENV USBPATH="/dev/ttyUSB0"
ENV MQTT_SERVER="localhost"
ENV MQTT_PORT="1883"
WORKDIR /opt/ozw/
EXPOSE 1983
VOLUME ["/opt/ozw/config/"]
ENTRYPOINT /usr/bin/catchsegv /ozwdaemon/bin/ozwdaemon -s $USBPATH -c /opt/ozw/config/ -u /opt/ozw/config/ --mqtt-server $MQTT_SERVER --mqtt-port $MQTT_PORT

