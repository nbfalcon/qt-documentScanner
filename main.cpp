#include "documentscanner.h"
#include "liveqml.h"
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme("document-scan"));

    DocumentScanner scanner{};

    QQmlApplicationEngine engine{};
    qtx::LiveQML liveEngine{&engine};
    liveEngine.setInitialProperties({{"scanner", QVariant::fromValue(&scanner)}});
    liveEngine.load("main.qml");

//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//    if (engine.rootObjects().isEmpty())
//        return -1;

    return app.exec();
}
