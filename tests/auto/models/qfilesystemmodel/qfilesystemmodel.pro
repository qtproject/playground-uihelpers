CONFIG += testcase

QT += uihelpers uihelpers-private
QT += core-private gui testlib

SOURCES += tst_qfilesystemmodel.cpp
TARGET = tst_qfilesystemmodel

win32:CONFIG += insignificant_test # QTBUG-24291
