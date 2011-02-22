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
HEADERS = MapParser.h \
    MapServer.h \
    NetServer.h \
    NetServerClient.h \
    PlayerServer.h \
    ../NetMessage.h \
    $${ROOT_PWD}common/constant.h
SOURCES = MapParser.cpp \
    MapServer.cpp \
    NetServer.cpp \
    NetServerClient.cpp \
    PlayerServer.cpp
