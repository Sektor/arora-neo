TEMPLATE = app
TARGET = arora-cacheinfo
DEPENDPATH += .
INCLUDEPATH += .

win32: CONFIG += console
mac:CONFIG -= app_bundle

QT += network

# Input
SOURCES += main.cpp

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
OBJECTS_DIR = $$PWD/.obj
