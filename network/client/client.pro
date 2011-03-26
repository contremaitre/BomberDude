ROOT_PWD = ../../
DEPENDENCY_LIBRARIES = common
include($${ROOT_PWD}lib_dep.pri)
QT += network \
    xml
CONFIG += DEBUG \
    staticlib
TEMPLATE = lib
INCLUDEPATH += . \
    $${ROOT_PWD}common
DEPENDPATH += . \
    $${ROOT_PWD}common
LIBS += -L$${ROOT_PWD}common \
    -lcommon
HEADERS = NetClient.h \
    MapClient.h \
    ../NetMessage.h \
    $${ROOT_PWD}common/constant.h \
    BombClient.h
SOURCES = NetClient.cpp \
    MapClient.cpp \
    BombClient.cpp
