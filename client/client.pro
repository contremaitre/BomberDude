DEPENDENCY_LIBRARIES = common
include(../lib_dep.pri)
QT += network xml
CONFIG += DEBUG \
    staticlib
TEMPLATE = lib
INCLUDEPATH += . \
    ../common 
DEPENDPATH += . \
    ../common
LIBS += -L../common \
    -lcommon
HEADERS = NetClient.h
SOURCES = NetClient.cpp
