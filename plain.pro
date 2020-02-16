TEMPLATE = app
CONFIG += console c++17
CONFIG += core
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        bfs1.cpp \
        main.cpp \
        oldMain.cpp

LIBS += -lpthread

HEADERS += \
    bfs1.h \
    date.h

QMAKE_CXXFLAGS += -msse4.2 -O0
