#-------------------------------------------------
#
# Project created by QtCreator 2019-01-08T22:43:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mapox2D
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

CONFIG += c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mapview.cpp \
    mapscene.cpp \
    texturelist.cpp \
    commands/deletetilecommand.cpp \
    commands/filltilecommand.cpp \
    tools/dataloader.cpp \
    tools/datasaver.cpp \
    dialogs/loadingmapdialog.cpp \
    dialogs/newmapdialog.cpp \
    tileitem.cpp \
    commands/pastecommand.cpp \
    commands/fillselectioncommand.cpp \
    commands/anchorcommand.cpp \
    commands/deleteselectioncommand.cpp \
    dialogs/importspritesheetdialog.cpp \
    commands/discardfloatselectioncommand.cpp \
    commands/removeitemfromtexturelistcommand.cpp

HEADERS += \
        mainwindow.h \
    mapview.h \
    mapscene.h \
    texturelist.h \
    commands/deletetilecommand.h \
    commands/filltilecommand.h \
    tools/stringtools.h \
    tools/dataloader.h \
    tools/datasaver.h \
    dialogs/loadingmapdialog.h \
    dialogs/newmapdialog.h \
    tools/utilitytools.h \
    tileitem.h \
    commands/pastecommand.h \
    commands/fillselectioncommand.h \
    commands/anchorcommand.h \
    commands/deleteselectioncommand.h \
    dialogs/importspritesheetdialog.h \
    commands/discardfloatselectioncommand.h \
    commands/removeitemfromtexturelistcommand.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    mapox2d.qrc

DISTFILES +=
