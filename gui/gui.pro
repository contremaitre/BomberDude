ROOT_PWD = ../
DEPENDENCY_LIBRARIES = common
include($${ROOT_PWD}lib_dep.pri)

QT += network xml
CONFIG += DEBUG
TEMPLATE = app
TARGET = ../QtBomberman
DEPENDPATH += . \
    ui \
    $${ROOT_PWD}common \
    $${ROOT_PWD}network/client \
    $${ROOT_PWD}network/server
INCLUDEPATH += . \
    $${ROOT_PWD}/common \
    $${ROOT_PWD}network/client \
    $${ROOT_PWD}network/server
LIBS += -L$${ROOT_PWD}common \
    -L$${ROOT_PWD}network/client \
    -L$${ROOT_PWD}network/server \
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
