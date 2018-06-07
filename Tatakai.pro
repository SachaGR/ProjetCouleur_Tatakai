#-------------------------------------------------
#
# Project created by QtCreator 2018-04-17T20:20:45
#
#-------------------------------------------------

QT       += core gui widgets opengl multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tatakai
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
win32 {
    win32-msvc* {
        LIBS     += opengl32.lib glu32.lib
    } else {
        LIBS     += -lopengl32 -lglu32
    }
}
unix {
        LIBS     += -lGL -lGLU
}

INCLUDEPATH += $$(OPENCV_DIR)\..\..\include
LIBS += -L$$(OPENCV_DIR)\lib \
    -lopencv_core2413 \
    -lopencv_highgui2413 \
    -lopencv_imgproc2413 \
    -lopencv_features2d2413 \
    -lopencv_calib3d2413 \
    -lopencv_objdetect2413 \

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    player.cpp \
    attack.cpp

HEADERS += \
        mainwindow.h \
    player.h \
    attack.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    ressources.qrc
