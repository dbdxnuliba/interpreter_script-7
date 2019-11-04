#-------------------------------------------------
#
# Project created by QtCreator 2019-04-10T11:36:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = robotTeachABB
TEMPLATE = app

TRANSLATIONS += zh_cn.ts en_gb.ts

CONFIG += c++17

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
    MainWidget/Functions/ProgramEditor/programeditordata.cpp \
        main.cpp \
    teachstate.cpp \
        widget.cpp \
    Hardware/hardware.cpp \
    MainWidget/mainwidget.cpp \
    MainWidget/UpperWidget/upperwidget.cpp \
    MainWidget/LowerWidget/lowerwidget.cpp \
    MainWidget/UpperWidget/alltasks.cpp \
    MainWidget/UpperWidget/eventlog.cpp \
    MainWidget/listboxobject.cpp \
    MainWidget/stackedwidobject.cpp \
    MainWidget/comboboxobject.cpp \
    MainWidget/Functions/functions.cpp \
    MainWidget/Functions/HotEdit/hotedit.cpp \
    MainWidget/windowobject.cpp \
    MainWidget/Functions/IntOut/intout.cpp \
    MainWidget/Functions/FileManager/filemanager.cpp \
    MainWidget/filemangerobject.cpp \
    MainWidget/Functions/BackupRecovery/backuprecovery.cpp \
    MainWidget/Functions/Calibration/calibration.cpp \
    MainWidget/Functions/AutoProduce/autoproduce.cpp \
    MainWidget/Functions/SystemInfo/systeminfo.cpp \
    MainWidget/Functions/ProgramData/programdata.cpp \
    MainWidget/Functions/ControlPanel/controlpanel.cpp \
    MainWidget/Functions/ControlPanel/control.cpp \
    MainWidget/Functions/ManualControl/manualcontrol.cpp \
    MainWidget/Functions/ProgramEditor/programeditor.cpp \
    MainWidget/Functions/ProgramEditor/progeditmain.cpp \
    MainWidget/textbrowserobject.cpp \
    MainWidget/commoncontrols.cpp

HEADERS += \
    MainWidget/Functions/ProgramEditor/programeditordata.h \
    teachstate.h \
        widget.h \
    Hardware/hardware.h \
    MainWidget/mainwidget.h \
    MainWidget/UpperWidget/upperwidget.h \
    MainWidget/LowerWidget/lowerwidget.h \
    MainWidget/UpperWidget/alltasks.h \
    MainWidget/UpperWidget/eventlog.h \
    MainWidget/listboxobject.h \
    MainWidget/stackedwidobject.h \
    MainWidget/comboboxobject.h \
    MainWidget/Functions/functions.h \
    MainWidget/Functions/HotEdit/hotedit.h \
    MainWidget/windowobject.h \
    MainWidget/Functions/IntOut/intout.h \
    MainWidget/Functions/FileManager/filemanager.h \
    MainWidget/filemangerobject.h \
    MainWidget/Functions/BackupRecovery/backuprecovery.h \
    MainWidget/Functions/Calibration/calibration.h \
    MainWidget/Functions/AutoProduce/autoproduce.h \
    MainWidget/Functions/SystemInfo/systeminfo.h \
    MainWidget/Functions/ProgramData/programdata.h \
    MainWidget/Functions/ControlPanel/controlpanel.h \
    MainWidget/Functions/ControlPanel/control.h \
    MainWidget/Functions/ManualControl/manualcontrol.h \
    MainWidget/Functions/ProgramEditor/programeditor.h \
    MainWidget/Functions/ProgramEditor/progeditmain.h \
    MainWidget/textbrowserobject.h \
    MainWidget/commoncontrols.h

RESOURCES += \
    Resource/resource.qrc

unix{
INCLUDEPATH += ../robotServer \
            ../robotServer/RobotStructure \
            ../robotServer/SystemLayer/LinuxAPI \
            ../robotServer/RobotInterpreter \

LIBS += -L. -Wl,--rpath=./ -lrobotServerExe
}

win32{
INCLUDEPATH += ../robotServer \
            ../robotServer/RobotStructure \
            ../robotServer/SystemLayer/WindowsAPI \
            ../robotServer/RobotInterpreter \

LIBS += -L. -Wl,--rpath=./ -lRobotClient
}
