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
					DBGFILE=$(findDBGFile $lib)
					if (( $? == 0)); then
						LIBS+=" $DBGFILE"
					fi
					LIBS+=" $RP"
					echo "Got Lib:" $RP " " $PKG " " $DBGFILE
				fi
			fi
			
		done
		echo $LIBS
		sentry-cli --auth-token $SENTRY_TOKEN upload-dif -o openzwave -p qt-openzwave $LIBS --wait
	}

