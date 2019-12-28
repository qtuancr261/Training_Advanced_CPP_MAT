QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    client.cpp \
    messages/MessageClientBase.cpp \
    messages/MessageClientReqRegister.cpp \
    roomchat.cpp \
    bufferwrapper.cpp \
    serverapp.cpp

HEADERS += \
    client.h \
    messages/MessageClientBase.h \
    messages/MessageClientReqRegister.h \
    messages/MessageDef.h \
    roomchat.h \
    bufferwrapper.h \
    serverapp.h

# Poco Library
#INCLUDEPATH += ../zpoco/inc
#LIBS += ../../zpoco/lib/libzpoco.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../poco/lib/release/ -lpocod
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../poco/lib/debug/ -lpocod
else:unix: LIBS += -L$$PWD/../poco/lib/ -lpocod

INCLUDEPATH += $$PWD/../poco/inc
DEPENDPATH += $$PWD/../poco/inc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../poco/lib/release/libpocod.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../poco/lib/debug/libpocod.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../poco/lib/release/pocod.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../poco/lib/debug/pocod.lib
else:unix: PRE_TARGETDEPS += $$PWD/../poco/lib/libpocod.a
