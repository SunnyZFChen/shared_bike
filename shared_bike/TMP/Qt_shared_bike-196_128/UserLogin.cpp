#include "UserLogin.h"
#include "bike.pb.h"
#include <QQmlApplicationEngine>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>


QTcpSocket* UserLogin::m_tcpClient = nullptr;
UserLogin* UserLogin::m_instance   = nullptr;
QObject*   UserLogin::m_qmlObject  = nullptr;
QString    UserLogin::m_userName   = "";
QString    UserLogin::m_userPwd    = "";

int   UserLogin::m_verCode=0;


UserLogin::UserLogin(QObject *parent)
    : QObject(parent)
{
    //连接信号与槽，接收qml端发送的信号
    connect(this, &UserLogin::getVerificationCode_sig, this, &UserLogin::getVerificationCode_slot);
    connect(this, &UserLogin::registerUser_sig,        this, &UserLogin::registerUser_slot);
    connect(this, &UserLogin::connectToServer_sig,     this, &UserLogin::connectToServer_slot);
    connect(this, &UserLogin::reconnectToServer_sig,   this, &UserLogin::reconnectToServer_slot);
    connect(this, &UserLogin::loginToBikeClient_sig,   this, &UserLogin::loginToBikeClient_slot);
}

UserLogin::~UserLogin()
{
    /*if(m_tcpClient) delete m_tcpClient;
    if(m_instance)  delete m_instance;
    if(m_qmlObject) delete m_qmlObject;*/
}

void UserLogin::distroyFreeMemory()
{
    //不能释放掉m_tcpClient，因为传递给了networkInterface的m_tcpClient
    if(m_instance)  delete m_instance;
    if(m_qmlObject) delete m_qmlObject;
}

void UserLogin::sendMsgToServer(EventID_t eventID)
{
    connectToServer();
    if(!m_tcpClient)
    {
        openQmlMsgWindow("请求失败，无法连接服务器！");
        return;
    }
    //std::string str = message.toStdString();
    std::string str_userName = m_userName.toStdString();
    const char* m_userName_char = str_userName.c_str();

    std::string str_userPwd = m_userPwd.toStdString();
    const char* m_userPwd_char = str_userPwd.c_str();

    char msg[1024];

    switch (eventID) {

        case EEVENTID_GET_MOBILE_CODE_REQ:
        {
            tutorial::mobile_request  mr;

            mr.set_username(m_userName_char);
            int len = mr.ByteSizeLong();
            memcpy(msg, "FBEB", 4);

            *(u16*)(msg + 4) = EEVENTID_GET_MOBILE_CODE_REQ;//EEVENTID_GET_MOBLIE_CODE_REQ
            *(i32*)(msg + 6) = len;
            qDebug()<<"*(u16*)(msg + 4)="<<*(u16*)(msg + 4);
            qDebug()<<"*(i32*)(msg + 6)=="<< *(i32*)(msg + 6);
            mr.SerializeToArray(msg+10, len);

            if(m_tcpClient->write(msg, len+10)!=-1)
            {
                //ui->send_Edit->clear();
            }
            m_tcpClient->flush();
            break;
        }
        //注册请求
        case EEVENTID_REGISTER_USER_REQ:
        {
            tutorial::registerUser_request  rr;
            rr.set_username(m_userName_char);
            rr.set_userpwd(m_userPwd_char);
            rr.set_vercode(m_verCode);
            int len = rr.ByteSizeLong();
            memcpy(msg, "FBEB", 4);

            *(u16*)(msg + 4) = EEVENTID_REGISTER_USER_REQ;//
            *(i32*)(msg + 6) = len;
            rr.SerializeToArray(msg+10, len);
            //发送头部
            if(m_tcpClient->write(msg, len+10)!=-1)
            {
                qDebug()<<"注册请求已发送！";
                //ui->send_Edit->clear();
            }

            m_tcpClient->flush();
            break;
        }
        case EEVENTID_LOGIN_REQ:
        {
            tutorial::login_request  lr;
            lr.set_username(m_userName_char);
            lr.set_userpwd(m_userPwd_char);
            int len = lr.ByteSizeLong();
            memcpy(msg, "FBEB", 4);

            *(u16*)(msg + 4) = EEVENTID_LOGIN_REQ;//
            *(i32*)(msg + 6) = len;
            lr.SerializeToArray(msg+10, len);
            //发送头部
            if(m_tcpClient->write(msg, len+10)!=-1)
            {
                //ui->send_Edit->clear();
            }

            m_tcpClient->flush();
            break;
        }
    }
}

void UserLogin::openQmlMsgWindow(QString msg)
{
    if(m_qmlObject) {
        //调用QML中函数
        QVariant returnedValue;
        QVariant message = msg;
        QMetaObject::invokeMethod(m_qmlObject, "openMsgWindow",
                                  Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, message));
    }
}

void UserLogin::refreshLoginWindow()
{
    if(m_qmlObject) {
        //调用QML中函数
        QMetaObject::invokeMethod(m_qmlObject, "refreshLoginPage");
    }
}

void UserLogin::connectToServer()
{
    //初始化TCP
    if(m_tcpClient)
    {
        return;
    }
    if(!m_tcpClient)
    {
        m_tcpClient = new QTcpSocket();   //实例化
    }
    m_tcpClient->abort();                 //取消原有连接
    m_tcpClient->connectToHost(shareBikeServer_IP, shareBikeServer_port); //IP以及端口号

    if (m_tcpClient->waitForConnected(2000))  // 连接成功
    {
        connect(m_tcpClient, &QTcpSocket::readyRead, this, &UserLogin::ReadData);
        connect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(ReadError(QAbstractSocket::SocketError)));
    }else{
        openQmlMsgWindow("无法连接服务器！");
        refreshLoginWindow();
        delete m_tcpClient;
        m_tcpClient = nullptr;
    }
}

void UserLogin::openPhoneNotifyWindow(QString notifyMsg)
{

    #ifdef ANDROID
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(notifyMsg);
    QAndroidJniObject::callStaticMethod<void> (
            "an/qt/qtWithJava/MyJavaClass",
            "notify","(Ljava/lang/String;)V",
            javaNotification.object<jstring>()
        );

    #endif
}

void UserLogin::getVerificationCode_slot(const QString& userName)
{
    m_userName = userName;
    sendMsgToServer(EEVENTID_GET_MOBILE_CODE_REQ);
}

void UserLogin::registerUser_slot(const QString& userName, const QString& password, const QString& verCode)
{
    m_userName = userName;
    m_userPwd = password;
    m_verCode = verCode.toInt();

    sendMsgToServer(EEVENTID_REGISTER_USER_REQ);
}

void UserLogin::connectToServer_slot()
{
    connectToServer();
}
void UserLogin::reconnectToServer_slot()
{
    connectToServer();
}

void UserLogin::loginToBikeClient_slot(const QString &Name, const QString &password)
{
    m_userName = Name;
    m_userPwd  = password;

    sendMsgToServer(EEVENTID_LOGIN_REQ);
}

void UserLogin::ReadData()
{
    char readMessage[1024];
    QString qStr="";
    int readLen = m_tcpClient->read(readMessage, 1024);
    readMessage[readLen]='\0';
    qDebug()<<"readLen:"<<readLen;
    if(readLen != -1)
    {

         if(strncmp(readMessage, "FBEB", 4) ==0)
         {
             u16 code = *(u16*)(readMessage + 4);
             i32 len = *(i32*)(readMessage + 6);
             qDebug()<<"ReadData()--code:"<<code;
             if(code==EEVENTID_GET_MOBILE_CODE_RSP)
             {
                 tutorial::mobile_response mr;
                 mr.ParseFromArray(readMessage+10, len);
                 m_verCode = mr.vercode();
                 QString data(mr.data().c_str());
                 if(mr.rescode()==ERRC_SUCCESS)
                 {
                     //openQmlMsgWindow(QString("验证码为: %1").arg(m_verCode));
                     openPhoneNotifyWindow(QString("您正在注册共享单车，验证码为: %1，请保密哦.").arg(m_verCode));
                     openQmlMsgWindow("验证码已发送，请留意信息通知");
                 }
                 else{
                     openQmlMsgWindow(QString("验证码获取失败，错误码[%1]").arg(code));
                 }
             }
             else if(code==EEVENTID_REGISTER_USER_RSP)
             {
                 tutorial::registerUser_response rr;
                 rr.ParseFromArray(readMessage+10, len);

                 //ui->textBrowserMsg->append(qStr);
                 if(rr.rescode()==ERRC_SUCCESS)
                 {
                     openQmlMsgWindow("账号注册成功！");
                     openQmlLoginPage();
                 }
                 else if(rr.rescode() == ERRC_INVALID_MSG)
                 {
                     openQmlMsgWindow("此用户名已被注册！");
                 }
                 else if(rr.rescode() == ERRC_INVALID_DATA){
                     openQmlMsgWindow("验证码错误！");
                 }
                 else openQmlMsgWindow(QString("注册时出错了！错误码[%1]").arg(rr.rescode()));
             }
             else if(code==EEVENTID_LOGIN_RSP)
             {
                 tutorial::login_response lr;
                 lr.ParseFromArray(readMessage+10, len);

                 //ui->textBrowserMsg->append(qStr);
                 if(lr.rescode()==ERRC_SUCCESS)
                 {                    
                     //断开m_tcpClient的信号与槽函数，否则，m_tcpClient再次连接相同的信号时，绑定的槽函数仍然是之前的槽函数，即无效连接
                     disconnect(m_tcpClient, &QTcpSocket::readyRead, this, &UserLogin::ReadData);
                     //disconnect(m_tcpClient, &QTcpSocket::disconnectFromHost, this, &UserLogin::disconnectSer);
                     disconnect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
                             this, SLOT(ReadError(QAbstractSocket::SocketError)));                   
                     //openQmlMainPage();
                     if(lr.userlevel() == 1)      openQmlBikeManagePage();
                     else if(lr.userlevel() == 0) openQmlMainPage();
                 }
                 else{
                     openQmlMsgWindow("用户名或密码错误！请您重新输入");
                     refreshLoginWindow();
                 }
             }
             m_userName = "";
             m_userPwd = "";
             m_verCode = 0;
         }

    }
}

void UserLogin::ReadError(QAbstractSocket::SocketError)
{
    openQmlMsgWindow("与服务器连接已断开！");
    //ui->textBrowserMsg->append("与服务器连接已断开！");
    if(m_tcpClient)
    {
        m_tcpClient->disconnectFromHost();
        delete  m_tcpClient;
        m_tcpClient = nullptr;
    }

}

UserLogin *UserLogin::getInstance()
{
    if(m_instance==nullptr)
    {
        m_instance = new UserLogin();
    }
    return m_instance;
}

QObject *UserLogin::getQmlObject()
{
    if(m_qmlObject)
        return m_qmlObject;
    else
        return nullptr;
}

QString UserLogin::getUserName()
{
    return m_userName;
}

void UserLogin::setQmlObject(QObject* qmlObject)
{
    m_qmlObject = qmlObject;
}

void UserLogin::openQmlMainPage()
{
    if(m_qmlObject) {
        //调用QML中函数
        QMetaObject::invokeMethod(m_qmlObject, "enterToMainPage");

    }
}

void UserLogin::openQmlBikeManagePage()
{
    if(m_qmlObject) {
        //调用QML中函数
        QMetaObject::invokeMethod(m_qmlObject, "enterToBikeManagePage");
    }
}

void UserLogin::openQmlLoginPage()
{
    if(m_qmlObject) {
        if(m_tcpClient) delete m_tcpClient;
        QMetaObject::invokeMethod(m_qmlObject, "returnToLoginPage");
        m_instance->connectToServer();
    }
}

QTcpSocket *UserLogin::getTcpClient()
{
    qDebug()<<"m_tcpClient: %p"<<m_tcpClient;
    if(m_tcpClient) return m_tcpClient;
    else
        return nullptr;
}




