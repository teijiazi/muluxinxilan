QT += widgets
QT += gui
QT += network
QT += core


LIBS += -lgdi32
LIBS += -lurlmon
LIBS += -lVersion
LIBS += -lWs2_32
LIBS += -lWinhttp

CONFIG += c++11 console
#CONFIG -= app_bundle

QMAKE_CXXFLAGS +=  -Wno-unused-parameter

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../baikePub/baseop.cpp \
        ../baikePub/dd.cpp \
        ../baikePub/shouci.cpp \
        ../baikePub/xiazaifangda.cpp \
        ../rizhi/httpget.cpp \
        ../rizhi/httpreq.cpp \
        ../rizhi/rizhi.cpp \
        ../rizhi/sslhttp.cpp \
        chaxun.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../baikePub/BaikeTypeData.h \
    ../baikePub/baseop.h \
    ../baikePub/dd.h \
    ../baikePub/shouci.h \
    ../baikePub/xiazaifangda.h \
    ../rizhi/httpget.h \
    ../rizhi/httpreq.h \
    ../rizhi/rizhi.h \
    ../rizhi/sslhttp.h \
    chaxun.h

RC_ICONS+=muluxinxilan.ico
#VERSION = 1.0.0.396
