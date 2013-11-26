#-------------------------------------------------
#
# Project created by QtCreator 2013-02-24T00:00:40
#
#-------------------------------------------------

QT       += core gui

TARGET = ActuatorControlInMicrosystem
TEMPLATE = app
CONFIG	+= qt warn_on release \
           no_keywords

LIBS += -L/usr/xenomai/lib -L/usr/lib -lnative -lxenomai

INCLUDEPATH += /usr/xenomai/include

SOURCES += main.cpp\
        mainwindow.cpp \
    hysteresisdialog.cpp \
    laitepinta.c \
    simplecontroller.cpp \
    scopedmutex.cpp \
    realtimecontroller.cpp \
    piezosensor.cpp \
    piezoactuator.cpp \
    hysteresissingleton.cpp \
    hysteresisgraph.cpp

HEADERS  += mainwindow.h \
    hysteresisdialog.h \
    laitepinta.h \
    actuator.h \
    simplecontroller.h \
    sensor.h \
    scopedmutex.h \
    realtimecontroller.h \
    piezosensor.h \
    piezoactuator.h \
    hysteresissingleton.h \
    controllerinterface.h \
    hysteresisgraph.h

FORMS    += mainwindow.ui \
    hysteresisdialog.ui \
    hysteresisgraph.ui

XENO=/usr/bin/xeno-config
