#-------------------------------------------------
#
# Project created by QtCreator 2018-02-15T22:01:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vktest
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    model.cpp \
    setup.cpp \
    controller.cpp

HEADERS += \
        mainwindow.h \
    model.h \
    setup.h \
    controller.h

FORMS += \
        mainwindow.ui \
    setup.ui \
    profile.ui



#CONFIG(debug, debug|release) {

#    DESTDIR         = $$OUT_PWD/Debug

#}else {

#    DESTDIR         = $$OUT_PWD/Release

#}



#QMAKE_POST_LINK += \
#        $$QMAKE_COPY_DIR $$PWD/ssleay32.dll $$DESTDIR $$escape_expand(\\n)
        #$$QMAKE_COPY_DIR $$PWD/libeay32.dll $$DESTDIR $$escape_expand(\\n)
LIBS += \
        "$$PWD/ssleay32.dll" \
    "$$PWD/libeay32.dll"
