TARGET = UiHelpers
MODULE = uihelpers # not src, as per project file
load(qt_module)
QPRO_PWD   = $$PWD
QT += core core-private gui gui-private quick-private qml-private #v8-private

include(../modules/qt_uihelpers.pri)


DEFINES += UIHELPERS_LIB
DEFINES += $$quote(QT_BEGIN_MOC_NAMESPACE=\"QT_USE_NAMESPACE QT_USE_NAMESPACE_UIHELPERS\")

unix|win32-g++*:QMAKE_PKGCONFIG_REQUIRES = QtCore QtGui

HEADERS += \
    uihelpersversion.h \
    uihelpersglobal.h

include(utils/utils.pri)
include(models/models.pri)
