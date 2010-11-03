# #####################################################################
# Automatically generated by qmake (2.01a) Wed Sep 15 17:40:47 2010
# #####################################################################
DEPENDENCY_LIBRARIES = common client server
include(../lib_dep.pri)
QT += network xml
CONFIG += DEBUG
TEMPLATE = app
TARGET = ../QtBomberman
DEPENDPATH += . \
    ui \
    ../common \
    ../client \
    ../server
INCLUDEPATH += . \
    ../common \
    ../client \
    ../server
LIBS += -L../common \
    -L../client \
    -L../server \
    -lcommon \
    -lclient \
    -lserver

# Input
HEADERS += QGraphicsSquareItem.h \
    GameArena.h \
    GamePlay.h \
    PixmapsItems.h \
    Settings.h \
    startUi.h
FORMS += ui/main_window.ui

SOURCES += QGraphicsSquareItem.cpp \
    GameArena.cpp \
    GamePlay.cpp \
    main.cpp \
    PixmapsItems.cpp \
    Settings.cpp \
    startUi.cpp
