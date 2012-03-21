#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <UiHelpers/UiCompletionModel>
#include <QtQml/QQmlContext>
#include <QStringListModel>

QStringListModel *modelFromFile(const QString& fileName)
{
    QStringList words;
    QFile file(fileName);

    file.open(QFile::ReadOnly);
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }
    file.close();

    return new QStringListModel(words);
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(resources);
    QGuiApplication app(argc, argv);

    UiHelpers::UiCompletionModel model;
    model.setCaseSensitivity(Qt::CaseInsensitive);
    model.setSourceModel(modelFromFile(":/countries.txt"));

    QQuickView v;
    v.rootContext()->setContextProperty("completionModel", &model);
    v.setWindowTitle(QObject::tr("Completion Model"));
    v.setSource(QUrl("qrc:/main.qml"));

    v.show();

    return app.exec();
}
