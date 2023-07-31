QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Graph-Map

TEMPLATE = app
VERSION = 0.1

CONFIG += c++17

# pass target and version to compiler
DEFINES += APP_NAME=\\\"$$TARGET\\\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += \
    src \
    src/ui \

SOURCES += \
    main.cpp \
    src/BFS.cpp \
    src/PopupMessage.cpp \
    src/RectItem.cpp \
    src/SearchGraph.cpp \
    src/ui/Application.cpp \
    src/ui/Mainwindow.cpp

HEADERS += \
    src/BFS.h \
    src/PopupMessage.h \
    src/RectItem.h \
    src/SearchGraph.h \
    src/ui/Application.h \
    src/ui/Mainwindow.h

FORMS += \
    src/ui/Mainwindow.ui
