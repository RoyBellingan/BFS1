TEMPLATE = app
CONFIG += console c++17
#CONFIG += core
#CONFIG += app_bundle
#CONFIG += qt

#QT -= gui

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
QMAKE_CXXFLAGS += -g -march=native
QMAKE_CXXFLAGS += -O3 -flto -fuse-ld=gold -fuse-linker-plugin
#QMAKE_CXXFLAGS += -fprofile-dir=/tmp/pgo -fprofile-generate=/tmp/pgo
#LIBS += -lgcov --coverage
#QMAKE_CXXFLAGS += -fprofile-use=/tmp/pgo -fprofile-correction


#QMAKE_CXXFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_CFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_LFLAGS+= -fsanitize=address -fno-omit-frame-pointer
