#-------------------------------------------------
#
# InferenceEngine
#
#-------------------------------------------------

QT -= gui

TARGET = InferenceEngine
TEMPLATE = lib

CONFIG += c++20
CONFIG += staticlib
CONFIG -= app_bundle

DEFINES += INFERENCEENGINE_STATIC
DEFINES += INFERENCEENGINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    inferencekernel.cpp \
    workspace.cpp

HEADERS += \
    inferencekernel.h \
    inferenceengine_global.h \
    workspace.h

RESOURCES += \
    resources.qrc


#-------------------------------------------------
# Project directories
#-------------------------------------------------

PROJECT_ROOT = $$clean_path($$PWD/..)

PROP_COMPILER_DIR = \
    $$clean_path($$PROJECT_ROOT/PROPCompilerBackup)


#-------------------------------------------------
# Build directory
#
# Release -> build-vscode
# Debug   -> build-vscode-debug
#-------------------------------------------------

CONFIG(debug, debug|release) {
    BUILD_ROOT = \
        $$clean_path($$PROJECT_ROOT/build-vscode-debug)
} else {
    BUILD_ROOT = \
        $$clean_path($$PROJECT_ROOT/build-vscode)
}


#-------------------------------------------------
# PROPCompilerBackup
#-------------------------------------------------

PROP_COMPILER_BUILD = \
    $$clean_path($$BUILD_ROOT/PROPCompilerBackup)

INCLUDEPATH += \
    $$PROP_COMPILER_DIR


#-------------------------------------------------
# Link PROPCompilerBackup
#-------------------------------------------------

win32-g++:CONFIG(debug, debug|release) {

    LIBS += \
        -L$$PROP_COMPILER_BUILD/debug \
        -lPROPCompilerBackup

    PRE_TARGETDEPS += \
        $$PROP_COMPILER_BUILD/debug/libPROPCompilerBackup.a
}

win32-g++:CONFIG(release, debug|release) {

    LIBS += \
        -L$$PROP_COMPILER_BUILD/release \
        -lPROPCompilerBackup

    PRE_TARGETDEPS += \
        $$PROP_COMPILER_BUILD/release/libPROPCompilerBackup.a
}