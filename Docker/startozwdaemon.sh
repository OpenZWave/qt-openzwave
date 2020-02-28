#!/bin/bash
shopt -s nocaseglob
OZW_ARGS=()
if [ ! -c "${USB_PATH:=/dev/ttyUSB0}" ]; then
	echo "USB Path \"$USB_PATH\" does not exist or is not a Character Device"
	exit 128
else
	OZW_ARGS+=(-s "$USB_PATH")
fi
OZW_ARGS+=(--config-dir "${OZW_CONFIG_DIR:-/opt/ozw/config}")
OZW_ARGS+=(--user-dir "${OZW_USER_DIR:-/opt/ozw/config}")
OZW_ARGS+=(--mqtt-server "${MQTT_SERVER:-localhost}")
OZW_ARGS+=(--mqtt-port "${MQTT_PORT:-1883}")
if [[ "${STOP_ON_FAILURE:=true}" == true ]]; then
	OZW_ARGS+=(--stop-on-failure)
elif [[ "$STOP_ON_FAILURE" != "false" ]]; then
	echo "Invalid value for STOP_ON_FAILURE: \"$STOP_ON_FAILURE\", expected true or false."
	exit 1
fi
if [[ "${MQTT_TLS:=false}" == true ]]; then
	OZW_ARGS+=(--mqtt-tls)
elif [[  "$MQTT_TLS" != false ]]; then
	echo "Invalid value for MQTT_TLS: \"$MQTT_TLS\", expected true or false."
	exit 1
fi
OZW_ARGS+=(--mqtt-instance "${OZW_INSTANCE:-1}")
if [ ! -z "${MQTT_USERNAME}" ]; then
	OZW_ARGS+=(--mqtt-username "$MQTT_USERNAME")
fi

echo "Executing: /usr/local/bin/ozwdaemon ${OZW_ARGS[@]} $@"
exec /usr/local/bin/ozwdaemon "${OZW_ARGS[@]}" "$@"
