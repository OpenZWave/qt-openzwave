#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T16:10:29
#
#-------------------------------------------------

TEMPLATE = subdirs

CONFIG += ordered silent

SUBDIRS += qt-openzwave \
    qt-openzwavedatabase \
    qt-ozwdaemon 

Examples {
	SUBDIRS += qt-simpleclient
}
win32 {
	SUBDIRS -= qt-openzwavedatabase
}
