#-------------------------------------------------
#
# SEViewBackup
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SEViewBackup
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += INFERENCEENGINE_STATIC


#-------------------------------------------------
# Sources
#-------------------------------------------------

SOURCES += \
    main.cpp \
    seview.cpp \
    subjectWidget.cpp \
    coverWidget.cpp \
    conclusionsWidget.cpp


#-------------------------------------------------
# Headers
#-------------------------------------------------

HEADERS += \
    seview.h \
    conclusionsWidget.h \
    coverWidget.h \
    subjectWidget.h


#-------------------------------------------------
# Forms
#-------------------------------------------------

FORMS += \
    seview.ui


#-------------------------------------------------
# Resources
#-------------------------------------------------

RESOURCES += \
    seresources.qrc


#-------------------------------------------------
# Project paths
#-------------------------------------------------

PROJECT_ROOT = $$clean_path($$PWD/..)

INFERENCE_ENGINE_DIR = \
    $$clean_path($$PROJECT_ROOT/InferenceEngine)

PROP_COMPILER_DIR = \
    $$clean_path($$PROJECT_ROOT/PROPCompilerBackup)

INFERENCE_ENGINE_BUILD = \
    $$clean_path($$INFERENCE_ENGINE_DIR/build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug)

PROP_COMPILER_BUILD = \
    $$clean_path($$PROP_COMPILER_DIR/build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug)


#-------------------------------------------------
# Include paths
#-------------------------------------------------

INCLUDEPATH += \
    $$INFERENCE_ENGINE_DIR \
    $$PROP_COMPILER_DIR


#-------------------------------------------------
# Debug
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


#-------------------------------------------------
# Release
#-------------------------------------------------

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