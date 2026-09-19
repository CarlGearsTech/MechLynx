TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    PROPCompilerBackup \
    InferenceEngine \
    SEViewBackup

PROPCompilerBackup.file = PROPCompilerBackup/PROPCompilerBackup.pro

InferenceEngine.file = InferenceEngine/InferenceEngine.pro
InferenceEngine.depends = PROPCompilerBackup

SEViewBackup.file = SEViewBackup/SEViewBackup.pro
SEViewBackup.depends = InferenceEngine