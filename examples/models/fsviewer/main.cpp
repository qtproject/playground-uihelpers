#include <QtCore/QDir>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtDeclarative/QDeclarativeContext>
#include <UiHelpers/UiFileSystemModel>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    UiHelpers::UiFileSystemModel model;
    model.setRootPath(QDir::tempPath());

    QQuickView v;
    v.rootContext()->setContextProperty("fsmodel", &model);

    v.setWindowTitle(QObject::tr("Dir View"));
    v.setResizeMode(QQuickView::SizeRootObjectToView);
    v.resize(640, 480);
    v.setSource(QUrl::fromLocalFile("main.qml"));

    v.show();

    return app.exec();
}
