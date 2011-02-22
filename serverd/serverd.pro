ROOT_PWD = ../
DEPENDENCY_LIBRARIES = common network/server
include($${ROOT_PWD}lib_dep.pri)

QT += network \
    xml
CONFIG += DEBUG
TEMPLATE = app
TARGET = ../Serverd
DEPENDPATH += . \
    $${ROOT_PWD}common \
    $${ROOT_PWD}network/server
INCLUDEPATH += . \
    $${ROOT_PWD}common \
    $${ROOT_PWD}network/server
LIBS += -L$${ROOT_PWD}common \
    -L$${ROOT_PWD}network/server \
    -lserver \
    -lcommon

# Input
HEADERS += Serverd.h
SOURCES += main_serverd.cpp \
    Serverd.cpp
