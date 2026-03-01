#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QLabel>
#include "global_def.h"
#include <QTimer>
#include <QObject>


QT_BEGIN_NAMESPACE


namespace Ui { class UserLogin; }


QT_END_NAMESPACE

class UserLogin : public QObject
{
    Q_OBJECT

public:
    UserLogin(QObject *parent = nullptr);
    ~UserLogin();

    void distroyFreeMemory();
public:

    void sendMsgToServer(EventID_t eventID);
    void openQmlMsgWindow(QString msg);
    void refreshLoginWindow();
    void connectToServer();
    void openPhoneNotifyWindow(QString notifyMsg);
signals:
    //发送信号给地图界面
    void sendloginSigToMapwindow(QString userName);

    //qml发送的信号，连接到服务器，获取验证码，注册，获取用户名，密码，重连
    void getVerificationCode_sig(const QString& userName);
    void registerUser_sig(const QString& userName, const QString& password, const QString& verCode);
    void connectToServer_sig();
    void reconnectToServer_sig();
    void loginToBikeClient_sig(const QString& userName, const QString& password);

private slots:
    //连接到服务器，获取验证码，注册，获取用户名，密码，重连等槽函数
    void getVerificationCode_slot(const QString& userName);
    void registerUser_slot(const QString& userName, const QString& password, const QString& verCode);
    void connectToServer_slot();
    void reconnectToServer_slot();
    void loginToBikeClient_slot(const QString& userName, const QString& password);

    //客户端tcp监听槽函数
    void ReadData();
    void ReadError(QAbstractSocket::SocketError);

public:
    static UserLogin* getInstance();
    static QObject* getQmlObject();
    static QString getUserName();
    //调用qml中函数的接口
    static void setQmlObject(QObject* qmlObject);
    static void openQmlMainPage();
    static void openQmlBikeManagePage();
    static void openQmlLoginPage();
    static QTcpSocket* getTcpClient();
private:

    static QTcpSocket* m_tcpClient;
    static QString m_userName;
    static QString m_userPwd;
    static i32 m_verCode;
    static UserLogin* m_instance;
    static QObject* m_qmlObject;

};
#endif // UserLogin_H
