QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QT += core network

# Добавляем модуль для работы с изображениями
QT += gui

RESOURCES += resources.qrc

SOURCES += \
    loginwindow_tabs.cpp \
    main_tabs.cpp \
    mainwindow_tabs.cpp \
    myclient.cpp \
    registerdialog.cpp \
    steganography.cpp

HEADERS += \
    loginwindow_tabs.h \
    mainwindow_tabs.h \
    myclient.h \
    registerdialog.h \
    steganography.h

FORMS += \
    dialog.ui \
    login.ui \
    mainwindow_tabs.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    ../../Downloads/Снимок экрана 2025-05-12 005038.png \
    ../../Downloads/Снимок экрана 2025-05-12 005038.png

