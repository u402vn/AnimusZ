#-------------------------------------------------
#
# Project created by QtCreator 2017-07-29T11:21:04
#
#-------------------------------------------------

QT       += core gui widgets network serialport sql multimediawidgets

TARGET = UAVSimulator
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17


SOURCES +=\
	UAVSimulator/UAVSimMainWindow.cpp \
	UAVSimulator/UAVSimMain.cpp \
        UAVSimulator/UAVSimDataSender.cpp \
        ApplicationSettingsImpl.cpp \
        ApplicationSettings.cpp \
        CamPreferences.cpp \
        Common/CommonUtils.cpp \
        Common/CommonData.cpp \
        Common/CommonWidgets.cpp \
        HardwareLink/lz4.c \
        TelemetryDataFrame.cpp \
        ConstantNames.cpp


HEADERS  +=\
	UAVSimulator/UAVSimMainWindow.h \
        UAVSimulator/UAVSimDataSender.h \
        ApplicationSettingsImpl.h \
        ApplicationSettings.h \
        CamPreferences.h \
        Common/CommonUtils.h \
        Common/CommonData.h \
        Common/CommonWidgets.h \
        HardwareLink/lz4.h \
        TelemetryDataFrame.h \
        ConstantNames.h

RESOURCES += \
    UAVSimulator/TestFiles/TestFiles.qrc

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -std=gnu++11
