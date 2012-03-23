TEMPLATE = lib
TARGET  = qmlundoframeworkplugin
TARGETPATH = Playground/UiHelpers/UndoFramework

QT += qml uihelpers

CONFIG += qt plugin

SOURCES += plugin.cpp

SOURCES += uiquickundocommands.cpp \
           uiquickundostack.cpp

HEADERS += uiquickundocommands_p.h \
           uiquickundostack_p.h


DESTDIR = $$QT.qml.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
