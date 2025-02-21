#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>


//Include the MOC version, not the header!
#include "moc_gaobject.cpp"



int main(int argc, char *argv[]) {
    GAObject goodasm;
    QGuiApplication app(argc, argv);

    //Default fixed-width font.
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);


    //First apple the goodasm adapter.
    engine.rootContext()->setContextProperty("goodasm", &goodasm);
    //Then set the font.
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);
    //Then load the QML module.
    engine.loadFromModule("goodasmapp", "Main");





    return app.exec();
}
