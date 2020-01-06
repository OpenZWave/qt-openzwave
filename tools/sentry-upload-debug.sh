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
#echo "deb http://deb.debian.org/debian-debug/ buster-debug main" > /etc/apt/sources.list.d/debug.list
#echo "deb http://deb.debian.org/debian-debug/ buster-proposed-updates-debug main" >> /etc/apt/sources.list.d/debug.list
#echo "deb http://deb.debian.org/debian-debug/ testing-debug main" >> /etc/apt/sources.list.d/debug.list
#echo "deb http://deb.debian.org/debian-debug/ unstable-debug main" >> /etc/apt/sources.list.d/debug.list
#echo "deb http://deb.debian.org/debian-debug/ experimental-debug main" >> /etc/apt/sources.list.d/debug.list
apt-get update
apt-get install -y curl
if [ ! -f /usr/local/bin/sentry-cli ]; then curl -sL https://sentry.io/get-cli/ | bash; fi
#apt-get install -t unstable libqt5core5a-dbgsym libqt5network5-dbgsym libqt5remoteobjects5-dbgsym
#rm /etc/apt/sources.list.d/debug.list

function findDBGFile() {
	local DBGFILE
	DBGFILE=`ls /usr/lib/debug/usr/$1* 2> /dev/null`
	if (( $? != 0)); then
		return 1
	fi
	echo $DBGFILE
	return 0
}




ldd $EXECUTABLE | awk '{print $3}' | 
	{
		while IFS= read lib
		do
			if [ -f "$lib" ]; then
				useable=`sentry-cli difutil check $lib | grep "Usable: yes"`
				if [ ! -z "$useable" ]; then
					RP=`realpath $lib`
					#DBGFILE=$(findDBGFile $lib)
					#if (( $? == 0)); then
					#	LIBS+=" $DBGFILE"
					#fi
					LIBS+=" $RP"
					echo "Got Lib:" $RP " " $PKG " " $DBGFILE
				fi
			fi
			
		done
		echo $LIBS /usr/local/bin/ozwdaemon
		sentry-cli --auth-token $SENTRY_TOKEN upload-dif -o openzwave -p qt-openzwave $LIBS --wait
	}

