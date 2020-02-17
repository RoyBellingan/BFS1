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

QMAKE_CXXFLAGS+= -fsanitize=address -fno-omit-frame-pointer
QMAKE_CFLAGS+= -fsanitize=address -fno-omit-frame-pointer
QMAKE_LFLAGS+= -fsanitize=address -fno-omit-frame-pointer
