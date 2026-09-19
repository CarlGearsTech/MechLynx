#-------------------------------------------------
#
# SEViewBackup
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SEViewBackup
TEMPLATE = app

CONFIG += c++20

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += INFERENCEENGINE_STATIC

SOURCES += \
    main.cpp \
    seview.cpp \
    subjectWidget.cpp \
    coverWidget.cpp \
    conclusionsWidget.cpp

HEADERS += \
    seview.h \
    conclusionsWidget.h \
    coverWidget.h \
    subjectWidget.h

FORMS += \
    seview.ui

RESOURCES += \
    seresources.qrc


#-------------------------------------------------
# Project directories
#-------------------------------------------------

PROJECT_ROOT = \
    $$clean_path($$PWD/..)

INFERENCE_ENGINE_DIR = \
    $$clean_path($$PROJECT_ROOT/InferenceEngine)

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
# Dependency build directories
#-------------------------------------------------

INFERENCE_ENGINE_BUILD = \
    $$clean_path($$BUILD_ROOT/InferenceEngine)

PROP_COMPILER_BUILD = \
    $$clean_path($$BUILD_ROOT/PROPCompilerBackup)


#-------------------------------------------------
# Include paths
#-------------------------------------------------

INCLUDEPATH += \
    $$INFERENCE_ENGINE_DIR \
    $$PROP_COMPILER_DIR


#-------------------------------------------------
# Link InferenceEngine + PROPCompilerBackup
#-------------------------------------------------

win32-g++:CONFIG(debug, debug|release) {

    LIBS += \
        -L$$INFERENCE_ENGINE_BUILD/debug \
        -lInferenceEngine

    LIBS += \
        -L$$PROP_COMPILER_BUILD/debug \
        -lPROPCompilerBackup

    PRE_TARGETDEPS += \
        $$INFERENCE_ENGINE_BUILD/debug/libInferenceEngine.a

    PRE_TARGETDEPS += \
        $$PROP_COMPILER_BUILD/debug/libPROPCompilerBackup.a
}

win32-g++:CONFIG(release, debug|release) {

    LIBS += \
        -L$$INFERENCE_ENGINE_BUILD/release \
        -lInferenceEngine

    LIBS += \
        -L$$PROP_COMPILER_BUILD/release \
        -lPROPCompilerBackup

    PRE_TARGETDEPS += \
        $$INFERENCE_ENGINE_BUILD/release/libInferenceEngine.a

    PRE_TARGETDEPS += \
        $$PROP_COMPILER_BUILD/release/libPROPCompilerBackup.a
}