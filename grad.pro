#-------------------------------------------------
#
# Project created by QtCreator 2015-01-11T15:14:10
#
#-------------------------------------------------

QT       += core gui

TARGET = 1_BasicRendering
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    window.cpp \
    renderable.cpp \
    face.cpp \
    mesh.cpp \
    vertex.cpp \
    objfile.cpp \
    halfedge.cpp \
    subdiv.cpp \
    mousehandler.cpp

HEADERS += \
    window.h \
    vertex.h \
    renderable.h \
    halfedge.h \
    face.h \
    mesh.h \
    objfile.h \
    subdiv.h \
    mousehandler.h

RESOURCES += \
    resources.qrc

DISTFILES +=

CONFIG += c++14
