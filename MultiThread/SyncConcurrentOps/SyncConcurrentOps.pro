TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp
LIBS += \
        -lpthread \
        -ldl \
        -lrt

HEADERS += \
    ThreadSafeQueue.h