ROOT_PWD = ../
DEPENDENCY_LIBRARIES = common \
    network/client
include($${ROOT_PWD}lib_dep.pri)
QT += network \
    xml
CONFIG += DEBUG
TEMPLATE = app
TARGET = ../QtBomberman
DEPENDPATH += . \
    ui \
    $${ROOT_PWD}common \
    $${ROOT_PWD}network/client
INCLUDEPATH += . \
    $${ROOT_PWD}/common \
    $${ROOT_PWD}network/client
LIBS += -L$${ROOT_PWD}common \
    -L$${ROOT_PWD}network/client \
    -lclient \
    -lcommon

# Input
HEADERS += QGraphicsSquareItem.h \
    GameArena.h \
    GamePlay.h \
    PixmapsItems.h \
    Settings.h \
    IpStats.h \
    MenuTabFrame.h \
    GameFrame.h \
    InterGameFrame.h \
    PlayerListWidget.h \
    KeySettingsWidget.h \
    startUi.h \
    qbomb.h \
    QFlame.h \
    QAnimateditem.h \
    QPlayer.h
    
FORMS += ui/MainWindow.ui \
    ui/MainMenuWidget.ui \
    ui/GameFrame.ui \
    ui/MenuTabFrame.ui \
    ui/InterGameFrame.ui \
    ui/PlayerListWidget.ui \
    ui/KeySettingsWidget.ui \
    ui/IpStats.ui

SOURCES += QGraphicsSquareItem.cpp \
    GameArena.cpp \
    GamePlay.cpp \
    main.cpp \
    PixmapsItems.cpp \
    Settings.cpp \
    IpStats.cpp \
    MenuTabFrame.cpp \
    GameFrame.cpp \
    InterGameFrame.cpp \
    PlayerListWidget.cpp \
    KeySettingsWidget.cpp \
    startUi.cpp \
    qbomb.cpp \
    QFlame.cpp \
    QAnimateditem.cpp \
    QPlayer.cpp
