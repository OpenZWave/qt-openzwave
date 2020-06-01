#!/bin/sh
mkdir -p /opt/ozw/config/logs/
mkdir -p /opt/ozw/config/crashes/
exec /usr/bin/supervisord -c /etc/supervisord.conf