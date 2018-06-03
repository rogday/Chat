#-------------------------------------------------
#
# Project created by QtCreator 2018-05-18T19:10:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += D:/alex/boost_1_66_0 \
            ../API/include

CONFIG += c++14

LIBS += -LD:\alex\boost_1_66_0\lib64-msvc-14.1 \
        -lboost_system-vc141-mt-x64-1_66

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        Client.cpp \
    ../API/src/Messages.cpp \
    ../API/src/Utils.cpp

HEADERS += \
        mainwindow.h \
        Client.h \
    ../API/include/Messages.h \
    ../API/include/Utils.h

FORMS += \
        mainwindow.ui
