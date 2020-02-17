TEMPLATE = app
CONFIG += console c++17
CONFIG += core
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        bfs1.cpp \
        main.cpp \
        oldMain.cpp


LIBS += -ljemalloc
LIBS += -lpthread

HEADERS += \
    bfs1.h \
    date.h

#QMAKE_CXXFLAGS += -O0
QMAKE_CXXFLAGS += -march=native
QMAKE_CXXFLAGS += -O3 -flto -fuse-ld=gold -fuse-linker-plugin

#QMAKE_CXXFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_CFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_LFLAGS+= -fsanitize=address -fno-omit-frame-pointer
