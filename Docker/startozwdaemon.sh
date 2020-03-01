#!/bin/bash
shopt -s nocaseglob
if [ ! -c "$USB_PATH" ]
then
	echo "USB Path \"$USB_PATH\" does not exist or is not a Character Device"
	exit 128
else
	CMD="-s $USB_PATH"
fi

if [ ! -z "$MQTT_SERVER" ]
then
	CMD+=" --mqtt-server $MQTT_SERVER"
	if [ ! -z "$MQTT_PORT" ]
	then
		CMD+=" --mqtt-port $MQTT_PORT"
	else
		CMD+=" --mqtt-port 1883"
	fi
fi
	
if [ -z "$STOP_ON_FAILURE" ]
then
	CMD+=" --stop-on-failure"
else 
	if [[ $STOP_ON_FAILURE != "false" ]]
	then
		CMD+=" --stop-on-failure"
	fi
fi

if [ ! -z "$MQTT_TLS" ]
then
	if [[ $MQTT_TLS == "true" ]]
	then
		CMD+=" --mqtt-tls"
	fi
fi


if [ -z "$OZW_INSTANCE" ] 
then
	CMD+=" --mqtt-instance 1"
else 
	CMD+=" --mqtt-instance $OZW_INSTANCE"
fi

if [ ! -z "$MQTT_USERNAME" ]
then
	CMD+=" --mqtt-username $MQTT_USERNAME"
fi


echo "Executing: /usr/local/bin/ozwdaemon $CMD"
/usr/local/bin/ozwdaemon $CMD

	