QT += core
QT -= gui

CONFIG += c++11

TARGET = qdmp6050
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ArduinoWrapper.cpp \
    edison_i2c/edison_i2c.cpp \
    edison_i2c/i2c.c \
    I2Cdev/I2Cdev.cpp \
    MPU6050/MPU6050.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += ./edison_i2c
INCLUDEPATH += ./I2Cdev
INCLUDEPATH += ./MPU6050
HEADERS += \
    ArduinoWrapper.h \
    edison_i2c/edison_i2c.h \
    edison_i2c/i2c.h \
    edison_i2c/i2c-dev-user.h \
    I2Cdev/I2Cdev.h \
    MPU6050/helper_3dmath.h \
    MPU6050/MPU6050.h \
    MPU6050/MPU6050_6Axis_MotionApps20.h \
    MPU6050/MPU6050_9Axis_MotionApps41.h

DISTFILES +=
