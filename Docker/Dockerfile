############################################################
# Dockerfile to build OpenZWave Library container images
# Based on CentOS7
############################################################

ARG distcchosts=''
ARG concurrency="12"
ARG qtversion="5.12.9"


# Set the base image to Alpine
FROM fishwaldo/ozw-base:${qtversion} as builder

# File Author / Maintainer
LABEL maintainer="justin@dynam.ac"


ENV LC_ALL C
ENV DEBIAN_FRONTEND noninteractive

WORKDIR /opt

#RUN echo 'APT::Default-Release "stable";' > /etc/apt/apt.conf.d/99defaultrelease \
#	&& echo 'deb     http://ftp.debian.org/debian/    unstable main contrib non-free' > /etc/apt/sources.list.d/unstable.list \
#	&& apt update
RUN apt update && apt-get -y install rapidjson-dev cmake pkgconf python libunwind-dev libcurl4-openssl-dev libx11-dev libglu1-mesa-dev libexecline-dev

ARG distcchosts=''
ARG concurrency="12"
ARG qtversion="5.12.9"

ENV DISTCC_HOSTS="$distcchosts"

RUN if [ -n "${DISTCC_HOSTS}" ]; then cd /usr/local/bin; ln /usr/bin/distcc gcc;  ln /usr/bin/distcc g++; echo "Using DistCC hosts ${DISTCC_HOSTS}"; fi

ENV PATH=$PATH:/opt/depot_tools/
ENV DEPOT_TOOLS_UPDATE=0
RUN git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git \
	&& cd depot_tools && git checkout 464e9ff && cd .. \
	&& mkdir breakpad \
	&& cd breakpad \
	&& fetch breakpad \
	&& cd src \
	&& ./configure --disable-processor --disable-tools \
	&& make -j${concurrency} \
	&& make install

RUN git clone https://github.com/OpenZWave/open-zwave.git \
	&& cd open-zwave \
	&& make -j${concurrency} \
	&& make install instlibdir=/usr/local/lib/

COPY . /opt/qt-openzwave/

ARG BP_CLIENTID
ARG BP_CLIENTKEY
ARG BUILDNUMBER=0
RUN cd qt-openzwave \
	&& if [ -f Makefile ]; then qmake -r; make distclean; fi \
	&& qmake -r CONFIG+=release "BP_CLIENTID=$BP_CLIENTID" "BP_CLIENTKEY=$BP_CLIENTKEY" "BUILDNUMBER=$BUILDNUMBER" \
	&& make -j${concurrency} \
	&& make install 

RUN git clone https://github.com/OpenZWave/ozw-admin.git \
	&& cd ozw-admin \
	&& if [ -f Makefile ]; then qmake -r; make distclean; fi \
	&& qmake -r CONFIG+=release \
	&& make -j${concurrency} \
	&& make install


ENV LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"

FROM debian:buster-slim as base

ARG qtversion="5.12.9"

ENV LC_ALL C
ENV DEBIAN_FRONTEND noninteractive

LABEL maintainer="justin@dynam.ac"

WORKDIR /opt

RUN apt update \
    && apt-get -y install libunwind8 libcurl4 binutils libglib2.0-0 libicu63 wget \
    && apt-get -y purge binutils \
    && apt-get -y autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* 

RUN case `uname -m` in x86_64) var="amd64";; armhf) var="armhf";; armv7l) var="armhf";; aarch64) var="aarch64";; esac && wget https://github.com/just-containers/s6-overlay/releases/download/v2.0.0.1/s6-overlay-$var.tar.gz -O /tmp/s6-overlay.tar.gz \
	&& tar xzf /tmp/s6-overlay.tar.gz -C / \
	&& rm /tmp/s6-overlay.tar.gz


COPY --from=builder /usr/local/bin/ozwdaemon /usr/local/bin/s6* /usr/local/bin/
COPY --from=builder /usr/local/lib/libopenzwave.so.1.6 /usr/local/lib/
COPY --from=builder /opt/qt/${qtversion}/lib/libQt5Core* /opt/qt/${qtversion}/lib/libQt5Network* /opt/qt/${qtversion}/lib/libQt5Mqtt* /opt/qt/${qtversion}/lib/libQt5WebSockets* /opt/qt/${qtversion}/lib/libQt5RemoteObjects* /opt/qt/${qtversion}/lib/
COPY --from=builder /opt/qt/${qtversion}/lib/libqt-openzwave* /opt/qt/${qtversion}/lib/
COPY --from=builder /opt/qt/${qtversion}/qt-openzwavedatabase.rcc /opt/qt/${qtversion}/

COPY Docker/base/ /

RUN ldconfig -v

ENV BP_DB_PATH="/opt/ozw/config/crashes/"
ENV S6_LOGGING_SCRIPT="n20 s10000000 T !\"gzip -nq9\""
WORKDIR /opt/ozw/
EXPOSE 1983
VOLUME ["/opt/ozw/config/"]
ENTRYPOINT ["/init"]

FROM base as allinone

ARG qtversion="5.12.9"

ENV LC_ALL C
ENV DEBIAN_FRONTEND noninteractive

ADD https://github.com/novnc/websockify/archive/v0.9.0.tar.gz /opt/source/
ADD https://github.com/novnc/noVNC/archive/v1.2.0.tar.gz /opt/source/

RUN apt update \
    && apt-get -y install libgl1 libpng16-16 libharfbuzz0b libfontconfig1 libmtdev1 libinput10 libxkbcommon0 libdbus-1-3 python3-setuptools python3-numpy \
    && apt-get -y autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
	&& cd /opt/source/ \
	&& tar -xzvf v0.9.0.tar.gz \
	&& cd websockify-0.9.0 \
	&& python3 setup.py install \
	&& cd /opt/source/ \
	&& tar -xzvf v1.2.0.tar.gz \
	&& cp -R /opt/source/noVNC-1.2.0/ /opt/novnc/ \
	&& rm -rf /opt/source/*


COPY --from=builder /usr/local/bin/ozwadmin /usr/local/bin/
COPY --from=builder /opt/qt/${qtversion}/lib/libQt5Svg* /opt/qt/${qtversion}/lib/libQt5Widgets* /opt/qt/${qtversion}/lib/libQt5Gui* /opt/qt/${qtversion}/lib/libQt5Xml* /opt/qt/${qtversion}/lib/libQt5SerialPort* /opt/qt/${qtversion}/lib/libQt5DBus* /opt/qt/${qtversion}/lib/
COPY --from=builder /opt/qt/${qtversion}/plugins /opt/qt/${qtversion}/plugins
COPY --from=builder /opt/qt/${qtversion}/resources /opt/qt/${qtversion}/resources
COPY --from=builder /opt/qt/${qtversion}/libexec /opt/qt/${qtversion}/libexec
COPY Docker/allinone/ /

ENV BP_DB_PATH="/opt/ozw/config/crashes/"
ENV VNC_PORT=5901
ENV WEB_PORT=7800
ENV QT_ASSUME_STDERR_HAS_CONSOLE=1
ENV S6_LOGGING_SCRIPT="n20 s10000000 T !\"gzip -nq9\""
WORKDIR /opt/ozw/
EXPOSE 1983
EXPOSE 5901
EXPOSE 7800
VOLUME ["/opt/ozw/config/"]

ENTRYPOINT ["/init"]
