#include <QtCore/QUrl>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
#include <UiHelpers/uihelpersglobal.h>
#include <UiHelpers/QUndoStack>

#include "stack.h"

QT_USE_NAMESPACE_UIHELPERS;

#include "main.moc"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView v;

    Stack stack;
    v.rootContext()->setContextProperty("stack", &stack);

    v.setSource(QUrl::fromLocalFile("main.qml"));

    v.show();

    return app.exec();
}

