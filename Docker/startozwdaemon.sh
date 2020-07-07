#!/usr/bin/env bash

: "${MQTT_PORT:=1883}"
: "${MQTT_SERVER:=localhost}"
: "${USB_PATH:=/dev/ttyUSB0}"

OZW_ARGS=()
if [[ ! -c $USB_PATH ]]; then
	echo "USB Path \"$USB_PATH\" does not exist or is not a Character Device"
	exit 128
fi
OZW_ARGS+=(-s "$USB_PATH")
OZW_ARGS+=(--config-dir "${OZW_CONFIG_DIR:-/opt/ozw/config}")
OZW_ARGS+=(--user-dir "${OZW_USER_DIR:-/opt/ozw/config}")
OZW_ARGS+=(--mqtt-server "$MQTT_SERVER")
OZW_ARGS+=(--mqtt-port "$MQTT_PORT")
if [[ ${STOP_ON_FAILURE:=true} == true ]]; then
	OZW_ARGS+=(--stop-on-failure)
elif [[ $STOP_ON_FAILURE != false ]]; then
	echo "Invalid value for STOP_ON_FAILURE: \"$STOP_ON_FAILURE\", expected true or false."
	exit 1
fi
if [[ ${MQTT_TLS:=false} == true ]]; then
	OZW_ARGS+=(--mqtt-tls)
elif [[ $MQTT_TLS != false ]]; then
	echo "Invalid value for MQTT_TLS: \"$MQTT_TLS\", expected true or false."
	exit 1
fi
OZW_ARGS+=(--mqtt-instance "${OZW_INSTANCE:-1}")
if [[ -n $MQTT_USERNAME ]]; then
	OZW_ARGS+=(--mqtt-username "$MQTT_USERNAME")
fi

echo "Waiting until the MQTT broker is available..."
if ! timeout "${MQTT_CONNECT_TIMEOUT:=30}" \
	bash -c "until echo > /dev/tcp/$MQTT_SERVER/$MQTT_PORT; do sleep 1; done" &>/dev/null
then
	echo "Could not connect to the MQTT broker after $MQTT_CONNECT_TIMEOUT seconds"
	exit 124
fi

set -- ozwdaemon "${OZW_ARGS[@]}" "$@"

echo "Executing:" "$@"
exec "$@"
