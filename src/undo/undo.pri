### Temporary to build QAction-dependent code
DEFINES += QT_NO_ACTION

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qundostack.h \
    $$PWD/qundogroup.h

HEADERS += \
    $$PWD/qundostack_p.h

SOURCES += \
    $$PWD/qundogroup.cpp \
    $$PWD/qundostack.cpp

