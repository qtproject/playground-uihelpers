QT.uihelpers.VERSION = 5.0.0
QT.uihelpers.MAJOR_VERSION = 5
QT.uihelpers.MINOR_VERSION = 0
QT.uihelpers.PATCH_VERSION = 0

QT.uihelpers.name = UiHelpers
QT.uihelpers.bins = $$QT_MODULE_BIN_BASE
QT.uihelpers.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/UiHelpers
QT.uihelpers.private_includes = $$QT_MODULE_INCLUDE_BASE/UiHelpers/$$QT.uihelpers.VERSION
QT.uihelpers.sources = $$QT_MODULE_BASE/src
QT.uihelpers.libs = $$QT_MODULE_LIB_BASE
QT.uihelpers.plugins = $$QT_MODULE_PLUGIN_BASE
QT.uihelpers.imports = $$QT_MODULE_IMPORT_BASE
QT.uihelpers.depends = core gui
QT.uihelpers.DEFINES = UIHELPERS_LIB

QT_CONFIG += uihelpers
