
#include <QtCore/QUrl>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QtWebView/QtWebView>
#include <QFileInfo>
#include <QUrl>
#include <QtWebChannel/QWebChannel>

#include "websocketclientwrapper.h"
#include "websockettransport.h"
#include "UserLogin.h"
#include "networkInterface.h"

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);
    //此函数用于初始化不同后端所需的资源或设置选项。
    //注意：需要在创建QGuiApplication实例后立即调用Initialize()函数
    QtWebView::initialize();
    QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main",
                                                                           "QtWebView Example"));

    QFileInfo jsFileInfo(":/qwebchannel.js");

    if (!jsFileInfo.exists()){
        qDebug()<<"qwebchannel.js no exists!";
        return -1;
    }


    // 设置QWebSocketServer
    QWebSocketServer server(QStringLiteral("QWebChannel Standalone Example Server"), QWebSocketServer::NonSecureMode);


    if (!server.listen(QHostAddress::LocalHost, 12345)) {
        qFatal("Failed to open web socket server.");
        return 1;
    }

    // 在QWebChannel AbstractTransport对象中包装WebSocket客户端
    WebSocketClientWrapper clientWrapper(&server);

    // 设置 channel
    QWebChannel channel(nullptr);
    QObject::connect(&clientWrapper, &WebSocketClientWrapper::clientConnected,
                     &channel, &QWebChannel::connectTo);

    // 设置 networkInterface 并将其发布到QWebChannel
    channel.registerObject(QStringLiteral("msg_interface"), networkInterface::getInstance());


    //注册C++类型UserLogin，便于在qml中调用UserLogin的方法
    qmlRegisterType<UserLogin>("UserLogin.module",1,0,"UserLogin");

    QQmlApplicationEngine engine;
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/main.qml")));

    QObject* object = component.create();
    //绑定Qml对象，便于对qml文件操作
    UserLogin::setQmlObject(object);
    UserLogin::getInstance()->connectToServer();

    return app.exec();
}
