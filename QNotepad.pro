QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AppInfo.cpp \
    Attr.cpp \
    Dialog.cpp \
    Editor.cpp \
    FileUtil.cpp \
    IconUtil.cpp \
    Lang.cpp \
    Main.cpp \
    MainWindow.cpp \
    MenuBar.cpp \
    StatusBar.cpp

HEADERS += \
    AppInfo.h \
    Attr.h \
    Dialog.h \
    Editor.h \
    FileUtil.h \
    IconUtil.h \
    Lang.h \
    MainWindow.h \
    MenuBar.h \
    StatusBar.h

include(SingleApplication-3.5.2/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

TARGET = "QNotepad"
VERSION = 2.1.0

win32 {
    RC_ICONS = "QNotepad.ico"
    QMAKE_TARGET_DESCRIPTION = "QNotepad"
} mac {
    ICON = "QNotepad.icns"
}

TRANSLATIONS += \
  QNotepad_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
  QNotepad.qrc
