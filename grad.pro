#-------------------------------------------------
#
# Project created by QtCreator 2015-01-11T15:14:10
#
#-------------------------------------------------

QT       += core gui

TARGET = GradMesh
CONFIG   -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

SOURCES += main.cpp \
    renderable/renderable.cpp \
    mesh/face.cpp \
    mesh/mesh.cpp \
    mesh/vertex.cpp \
    objfile.cpp \
    mesh/halfedge.cpp \
    subdiv.cpp \
    renderable/meshrenderable.cpp \
    mainwindow.cpp \
    controlwidget.cpp \
    mesh/meshhandler.cpp \
    mousehandler.cpp \
    renderable/selectionrenderable.cpp

HEADERS += \
    mesh/vertex.h \
    renderable/renderable.h \
    mesh/halfedge.h \
    mesh/face.h \
    mesh/mesh.h \
    objfile.h \
    subdiv.h \
    renderable/meshrenderable.h \
    mainwindow.h \
    controlwidget.h \
    mesh/meshhandler.h \
    mousehandler.h \
    renderable/selectionrenderable.h

RESOURCES += \
    resources.qrc

DISTFILES +=

CONFIG += c++14

FORMS += \
    controlwidget.ui
