# #####################################################################
# Automatically generated by qmake (2.01a) Wed Sep 15 17:40:47 2010
# #####################################################################
DEPENDENCY_LIBRARIES = src
include(../lib_dep.pri)
QT += network
CONFIG += DEBUG
TEMPLATE = app
TARGET = ../QtBomberman
DEPENDPATH += . \
    ui \
    ../src
INCLUDEPATH += . \
    ../src
LIBS += -L../src \
    -lsrc

# Input
HEADERS += QGraphicsSquareItem.h \
    GameArena.h \
    GamePlay.h \
    PixmapsItems.h \
    Settings.h \
    startUi.h
FORMS += ui/main_window.ui \
    ui/network.ui
SOURCES += QGraphicsSquareItem.cpp \
    GameArena.cpp \
    GamePlay.cpp \
    main.cpp \
    PixmapsItems.cpp \
    Settings.cpp \
    startUi.cpp
