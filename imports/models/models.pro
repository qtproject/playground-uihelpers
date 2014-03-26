TEMPLATE = lib
TARGET  = qmlmodelsplugin
TARGETPATH = Playground/UiHelpers/Models

QT += qml uihelpers

CONFIG += qt plugin

SOURCES += plugin.cpp

SOURCES += uiquickcompletionmodel.cpp

HEADERS += uiquickcompletionmodel_p.h

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
