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

#include "application/CellService.h"
#include "application/TransactionService.h"
#include "application/UserService.h"
#include "security/PasswordHasher.h"
#include "storage/sqlite/SQLiteCategoryRepository.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"
#include "ui/qt/controllers/CellController.h"
#include "ui/qt/controllers/MemberController.h"
#include "ui/qt/controllers/UserController.h"
#include "ui/qt/session/SessionState.h"

int main(int argc, char* argv[])
{
    QGuiApplication application(argc, argv);
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/assets/FinanCellIcon.png")));

    std::unique_ptr<SQLiteDatabase> database;
    std::unique_ptr<SQLiteUserRepository> userRepository;
    std::unique_ptr<SodiumPasswordHasher> passwordHasher;
    std::unique_ptr<UserService> userService;
    std::unique_ptr<SQLiteCellRepository> cellRepository;
    std::unique_ptr<CellService> cellService;
    std::unique_ptr<SQLiteCategoryRepository> categoryRepository;
    std::unique_ptr<SQLiteTransactionRepository> transactionRepository;
    std::unique_ptr<TransactionService> transactionService;
    std::unique_ptr<SessionState> session;
    std::unique_ptr<UserController> userController;
    std::unique_ptr<CellController> cellController;
    std::unique_ptr<MemberController> memberController;
    QString startupError;

    try
    {
        std::filesystem::create_directories("data");
        database = std::make_unique<SQLiteDatabase>("data/financell.db");
        SQLiteMigrations::apply(*database);
        userRepository = std::make_unique<SQLiteUserRepository>(*database);
        passwordHasher = std::make_unique<SodiumPasswordHasher>();
        userService = std::make_unique<UserService>(*userRepository, *passwordHasher);
        cellRepository = std::make_unique<SQLiteCellRepository>(*database);
        cellService = std::make_unique<CellService>(*cellRepository, *userRepository);
        categoryRepository = std::make_unique<SQLiteCategoryRepository>(*database);
        transactionRepository =
            std::make_unique<SQLiteTransactionRepository>(*database);
        transactionService = std::make_unique<TransactionService>(
            *transactionRepository, *cellRepository, *categoryRepository);
        session = std::make_unique<SessionState>();
        userController = std::make_unique<UserController>(*userService, *session);
        cellController = std::make_unique<CellController>(
            *cellService, *transactionService, *session);
        memberController = std::make_unique<MemberController>(*cellService, *session);
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
        QStringLiteral("cellController"), cellController.get());
    engine.rootContext()->setContextProperty(
        QStringLiteral("memberController"), memberController.get());
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
#include "storage/sqlite/SQLiteCellRepository.h"
