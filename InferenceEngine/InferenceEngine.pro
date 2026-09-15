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

DEFINES += INFERENCEENGINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


#-------------------------------------------------
# Sources
#-------------------------------------------------

SOURCES += \
    inferencekernel.cpp \
    workspace.cpp \
    main.cpp


#-------------------------------------------------
# Headers
#-------------------------------------------------

HEADERS += \
    inferencekernel.h \
    inferenceengine_global.h \
    workspace.h


#-------------------------------------------------
# Resources
#-------------------------------------------------

RESOURCES += \
    resources.qrc


#-------------------------------------------------
# PROPCompilerBackup
#-------------------------------------------------

PROP_COMPILER_DIR = $$clean_path($$PWD/../PROPCompilerBackup)

PROP_COMPILER_BUILD = $$clean_path($$PROP_COMPILER_DIR/build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug)


#-------------------------------------------------
# Include path
#-------------------------------------------------

INCLUDEPATH += \
    $$PROP_COMPILER_DIR


#-------------------------------------------------
# Debug
#-------------------------------------------------

win32-g++:CONFIG(debug, debug|release) {
    LIBS += -L$$PROP_COMPILER_BUILD/debug \
            -lPROPCompilerBackup

    PRE_TARGETDEPS += \
        $$PROP_COMPILER_BUILD/debug/libPROPCompilerBackup.a
}


#-------------------------------------------------
# Release
#-------------------------------------------------

win32-g++:CONFIG(release, debug|release) {
    LIBS += -L$$PROP_COMPILER_BUILD/release \
            -lPROPCompilerBackup

    PRE_TARGETDEPS += \
        $$PROP_COMPILER_BUILD/release/libPROPCompilerBackup.a
}
