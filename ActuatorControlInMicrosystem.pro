#-------------------------------------------------
#
# Project created by QtCreator 2013-02-24T00:00:40
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ActuatorControlInMicrosystem
TEMPLATE = app
CONFIG	+= qt warn_on release \
           no_keywords

LIBS += -L/usr/xenomai/lib -L/usr/lib -lanalogy -lnative -lrtdm -lxenomai

INCLUDEPATH += /usr/xenomai/include

SOURCES += main.cpp\
        mainwindow.cpp \
    laitepinta.c \
    simplecontroller.cpp \
    realtimecontroller.cpp \
    piezosensor.cpp \
    piezoactuator.cpp

HEADERS  += mainwindow.h \
    laitepinta.h \
    actuator.h \
    simplecontroller.h \
    sensor.h \
    realtimecontroller.h \
    piezosensor.h \
    piezoactuator.h \
    controllerinterface.h

FORMS    += mainwindow.ui
