QT += core gui qml quick
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# KF5
# QT += KWidgetsAddons KXmlGui

CONFIG += c++17

LIBS += -lsane

# QML_ELEMENT
CONFIG += qmltypes
QML_IMPORT_NAME = org.nbfalcon.documentScanner
QML_IMPORT_MAJOR_VERSION = 1
QML_IMPORT_MINOR_VERSION = 0

CONFIG += sanitizer sanitize_address

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    documentscanner.cpp \
    main.cpp \
    qscannerinterface.cpp \
    scannedpage.cpp

HEADERS += \
    documentscanner.h \
    qscannerinterface.h \
    scannedpage.h

RESOURCES += \
    qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
