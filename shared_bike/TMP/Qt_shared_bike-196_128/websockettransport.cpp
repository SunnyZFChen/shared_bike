
#include "websockettransport.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtWebSockets/QtWebSockets>

/*
    在内部使用QWebSocket的简要QWebSocket实现

    传输将通过QWebSocket接收的所有消息委托给textMessageReceived信号。
    类似地，对sendTextMessage的所有调用都将通过QWebSocket发送到远程客户端。
*/

/*
    构造传输对象并包装给定的套接字。

    套接字也被设置为传输对象的父对象。
*/
WebSocketTransport::WebSocketTransport(QWebSocket *socket): QWebChannelAbstractTransport(socket)
, m_socket(socket)
{
    connect(socket, &QWebSocket::textMessageReceived,
            this, &WebSocketTransport::textMessageReceived);
    connect(socket, &QWebSocket::disconnected,
            this, &WebSocketTransport::deleteLater);
}

/*
    销毁 WebSocketTransport.
*/
WebSocketTransport::~WebSocketTransport()
{
    m_socket->deleteLater();

}

/*
    序列化JSON消息，并通过WebSocket将其作为文本消息发送到qt客户端。
*/
void WebSocketTransport::sendMessage(const QJsonObject &message)
{
    QJsonDocument doc(message);
    m_socket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

/*
    反序列化字符串化的JSON messageData并发出messageReceived。
*/
void WebSocketTransport::textMessageReceived(const QString &messageData)
{
    QJsonParseError error;
    QJsonDocument message = QJsonDocument::fromJson(messageData.toUtf8(), &error);
    if (error.error) {
        qWarning() << "Failed to parse text message as JSON object:" << messageData
                   << "Error is:" << error.errorString();
        return;
    } else if (!message.isObject()) {
        qWarning() << "Received JSON message that is not an object: " << messageData;
        return;
    }
    emit messageReceived(message.object(), this);
}
