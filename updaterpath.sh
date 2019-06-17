#!/bin/sh
OZWPATH=`otool -L $1  | grep libopenzwave | awk '{print $1}'`
install_name_tool -change $OZWPATH  @rpath/libopenzwave-1.6.dylib $1
