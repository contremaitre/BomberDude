DEPENDENCY_LIBRARIES = common
include(../lib_dep.pri)
QT += network \
    xml
CONFIG += DEBUG \
    staticlib
TEMPLATE = lib
INCLUDEPATH += . \
    ../common
DEPENDPATH += . \
    ../common
LIBS += -L../common \
    -lcommon
HEADERS = MapParser.h \
    MapServer.h \
    NetServer.h \
    NetServerClient.h 
SOURCES = MapParser.cpp \
    MapServer.cpp \
    NetServer.cpp \
    NetServerClient.cpp 