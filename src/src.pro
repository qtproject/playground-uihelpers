load(qt_module)

TARGET = UiHelpers
QPRO_PWD   = $$PWD
QT = core core-private gui
CONFIG += module
MODULE_PRI = ../modules/qt_uihelpers.pri

MODULE = uihelpers
load(qt_module_config)

DEFINES += UIHELPERS_LIB
DEFINES += $$quote(QT_BEGIN_MOC_NAMESPACE=\"QT_USE_NAMESPACE QT_USE_NAMESPACE_UIHELPERS\")

unix|win32-g++*:QMAKE_PKGCONFIG_REQUIRES = QtCore QtGui

HEADERS += \
    uihelpersversion.h \
    uihelpersglobal.h

include(undo/undo.pri)
include(models/models.pri)
