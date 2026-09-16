#TEMPLATE = app
TEMPLATE = lib
CONFIG += staticlib
#CONFIG += c++11
#CONFIG += console c++11
#CONFIG -= app_bundle
#CONFIG += qt

SOURCES += propnode.cpp \
    proptreebuilder.cpp \
    proplexanalyzer.cpp \
    rpnsyntaxanalyzer.cpp \
    infixsyntaxanalyzer.cpp \
    propsyntaxanalyzer.cpp

HEADERS += \
    propnode.h \
    proptreebuilder.h \
    proplexanalyzer.h \
    propsyntaxanalyzer.h \
    rpnsyntaxanalyzer.h \
    infixsyntaxanalyzer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    resources.qrc
