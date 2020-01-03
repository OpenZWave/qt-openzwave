#!/bin/sh
OZWPATH=`otool -L $1  | grep libopenzwave | grep compatibility | awk '{print $1}'`
echo $OZWPATH
install_name_tool -change $OZWPATH  @rpath/libopenzwave-1.6.dylib $1
