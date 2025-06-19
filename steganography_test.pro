QT += core gui

CONFIG += c++17 console
CONFIG -= app_bundle

# Добавляем модуль для работы с изображениями
QT += gui

SOURCES += \
    steganography.cpp \
    steganography_test.cpp

HEADERS += \
    steganography.h

# Добавляем библиотеку OpenSSL для шифрования
unix|win32: LIBS += -lssl -lcrypto

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

