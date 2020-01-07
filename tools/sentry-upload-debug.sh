#!/bin/bash
EXECUTABLE=$1

if [ -z $EXECUTABLE ]
then
	echo "Please Supply a Executable to process"
	exit -1
fi

if [ -z $SENTRY_TOKEN ]
then
	echo "Please Set the SENTRY_TOKEN enviroment variable"
	exit -1
fi
echo "deb http://deb.debian.org/debian-debug/ buster-debug main" > /etc/apt/sources.list.d/debug.list
echo "deb http://deb.debian.org/debian-debug/ buster-proposed-updates-debug main" >> /etc/apt/sources.list.d/debug.list
echo "deb http://deb.debian.org/debian-debug/ testing-debug main" >> /etc/apt/sources.list.d/debug.list
echo "deb http://deb.debian.org/debian-debug/ unstable-debug main" >> /etc/apt/sources.list.d/debug.list
echo "deb http://deb.debian.org/debian-debug/ experimental-debug main" >> /etc/apt/sources.list.d/debug.list
if [ ! -f /usr/local/bin/sentry-cli ]; then
	apt-get update
	apt-get install -y curl elfutils
	apt-get install -y -t unstable libqt5core5a-dbgsym libqt5network5-dbgsym libqt5remoteobjects5-dbgsym
	ARCH=`uname -m`
	if [ "$ARCH" == "aarch64" ]; then
		echo "Installing sentry-cli for arm64"
 		curl http://bamboo.my-ho.st/bamboo/browse/INFRA-SCL/latest/artifact/shared/sentry-cli-arm64/sentry-cli -o /usr/local/bin/sentry-cli
		chmod +x /usr/local/bin/sentry-cli
	elif [ "$ARCH" == "armv7l" ]; then
		echo "Installing sentry-cli for armhf"
		curl http://bamboo.my-ho.st/bamboo/browse/INFRA-SCL/latest/artifact/shared/sentry-cli-armhf/sentry-cli -o /usr/local/bin/sentry-cli
		chmod +x /usr/local/bin/sentry-cli
	else
		echo "Installing sentry-cli for $ARCH"
		curl -sL https://sentry.io/get-cli/ | bash
	fi
fi

function findDBGFile() {
	local DBGFILE
	DBGFILE=`eu-unstrip -n -e $1 | awk '{print $4}'`
	if (( $? != 0)); then
		return 1
	fi
	if [ "$DBGFILE" == "-" ]; then
		return 1
	elif [ "$DBGFILE" == "." ]; then
		return 1
	else 
		echo $DBGFILE
		return 0
	fi
}




ldd $EXECUTABLE | awk '{print $3}' | 
	{
		while IFS= read lib
		do
			if [ -f "$lib" ]; then
				useable=`sentry-cli difutil check $lib | grep "Usable: yes"`
				if [ ! -z "$useable" ]; then
					RP=`realpath $lib`
					DBGFILE=$(findDBGFile $lib)
					if (( $? == 0)); then
						LIBS+=" $DBGFILE"
					fi
					LIBS+=" $RP"
					echo "Got Lib:" $RP " " $DBGFILE
				fi
			fi
			
		done
		LIBS+=" /usr/local/lib/ozwdaemon"
		echo $LIBS
		sentry-cli --auth-token $SENTRY_TOKEN upload-dif -o openzwave -p qt-openzwave $LIBS --wait
	}

