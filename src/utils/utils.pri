### Temporary to build QAction-dependent code
DEFINES += QT_NO_ACTION

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/uiundostack.h \
    $$PWD/uiundogroup.h \
    $$PWD/uiundostack_p.h

SOURCES += \
    $$PWD/uiundogroup.cpp \
    $$PWD/uiundostack.cpp

