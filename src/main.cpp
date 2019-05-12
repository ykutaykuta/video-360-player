//#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QTemporaryDir>
#include <QTimer>
#include <QThread>
#include <QApplication>

#include "videohandle.h"
#include "openglqml.h"
#include "ffmpegdecode.h"
#include "controller.h"

int main(int argc, char *argv[])
{  
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	QCoreApplication::setApplicationName("Video 360 player");

	QApplication app(argc, argv);

	QQmlApplicationEngine engine;

    qmlRegisterType<OpenGLQml>("CustomType", 1, 0, "OpenGLQML");

	Controller ctr(&engine);

//	VideoHandle video(&engine);

//	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &video, &VideoHandle::init);

	engine.load(QUrl(QStringLiteral("qrc:/res/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

//	QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);

    return app.exec();
}
