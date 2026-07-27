#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>
#include <QDebug>
#include <QIcon>
#include <QString>
#include <QUrl>

#include <exception>
#include <filesystem>
#include <memory>

#include "application/UserService.h"
#include "security/PasswordHasher.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteUserRepository.h"
#include "ui/qt/controllers/UserController.h"

int main(int argc, char* argv[])
{
    QGuiApplication application(argc, argv);
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/assets/FinanCellIcon.png")));

    std::unique_ptr<SQLiteDatabase> database;
    std::unique_ptr<SQLiteUserRepository> userRepository;
    std::unique_ptr<SodiumPasswordHasher> passwordHasher;
    std::unique_ptr<UserService> userService;
    std::unique_ptr<UserController> userController;
    QString startupError;

    try
    {
        std::filesystem::create_directories("data");
        database = std::make_unique<SQLiteDatabase>("data/financell.db");
        SQLiteMigrations::apply(*database);
        userRepository = std::make_unique<SQLiteUserRepository>(*database);
        passwordHasher = std::make_unique<SodiumPasswordHasher>();
        userService = std::make_unique<UserService>(*userRepository, *passwordHasher);
        userController = std::make_unique<UserController>(*userService);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "FinanCell GUI startup failed:" << error.what();
        startupError = QStringLiteral(
            "FinanCell could not open or prepare its database. Please check the data "
            "directory and try again.");
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
        QStringLiteral("userController"), userController.get());
    engine.rootContext()->setContextProperty(
        QStringLiteral("startupError"), startupError);
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
