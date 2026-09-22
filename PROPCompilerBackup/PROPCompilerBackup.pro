#-------------------------------------------------
#
# PROPCompilerBackup
#
#-------------------------------------------------

QT += core gui

TARGET = PROPCompilerBackup
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++23

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    propnode.cpp \
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
