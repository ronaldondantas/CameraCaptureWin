#-------------------------------------------------
#
# Project created by QtCreator 2016-02-02T18:28:58
#
#-------------------------------------------------

QT += core gui widgets multimedia multimediawidgets

TARGET = CameraCaptureWin
TEMPLATE = app

INCLUDEPATH += ./include
LIBS += -LC:/usr/local/lib

unix {
    DEFINES += QTMEL_NOT_INSTALLED
}

CONFIG(debug, debug|release) {
    win32: LIBS += -lqtmeld1
    unix: LIBS += -lqtmeld
} else {
    win32: LIBS += -lqtmel1
    unix: LIBS += -lqtmel
}

SOURCES += main.cpp \
    camerarecordwidget.cpp

HEADERS  += \
    camerarecordwidget.h

FORMS    += \
    camerarecordwidget.ui

RESOURCES += \
    icons.qrc
