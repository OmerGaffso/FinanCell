#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QObject>
#include <QUrl>

int main(int argc, char* argv[])
{
    QGuiApplication application(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl mainUrl(QStringLiteral("qrc:/qt/qml/FinanCell/Main.qml"));

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &application,
        [mainUrl](QObject* object, const QUrl& objectUrl)
        {
            if (!object && objectUrl == mainUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(mainUrl);
    return application.exec();
}
