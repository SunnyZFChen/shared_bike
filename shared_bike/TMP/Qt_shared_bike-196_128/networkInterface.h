#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include <QObject>
#include <QTcpSocket>
#include <QMap>
#include <QTimer>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>

#include "UserLogin.h"

class networkInterface : public QObject
{
    Q_OBJECT
private:
    networkInterface(QObject *parent = nullptr);

public:
    void startScanQRCode(QString name);
    //关于java的注册
    static void registerNatives();
    static void sendDataToQt(JNIEnv *env, jobject type, jstring jdata);
    static networkInterface* getInstance();

public:
    void initMyNumber();
    void connectServer();
    void setTcpClient(QTcpSocket *tcpclient);
    void ReadData();
    void ReadMessageFromServer(u32 id);

signals:
    /*
        这个信号是从qt端发出的，与HTML客户端交互
    */
    void ServerConnectSuccessSig();
    void ServerConnectFalseSig(QString falseMsg);
    void ListAllBikeMsgSig(const int bikeid, const int status, const double longitude, const double latitude);
    //void msgTipPrintToHtmlSig(QString tipMsg);
    void msgTipPrintToHtmlSig(QString tipMsg);
    void isSuccess(bool success);

    void sendBikeStartResSig(); //单车开始用车响应信号
    void sendBikeEndResSig();   //单车还车响应信号

    void addBikeResSig(const int bikeid, const double longitude, const double latitude);      //添加单车响应信号
    void deleteBikeResSig(const int bikeid);   //删除单车响应信号

    void bikeInfoBroadcastReqSig(const int bikeid, const int status, const double longitude, const double latitude);
    void bikeInfoBroadcastResSig(const int bikeid, const int status, const double longitude, const double latitude);

public slots:

    //注意：在与html的交互中不能使用i32，u16等类型的参数传入，因为javascript没有这些类型，导致无法识别
    /*
        从HTML客户端调用这个槽函数，实现与qt端的交互.
    */
    void ReadError(QAbstractSocket::SocketError);
    void connectBikeServer();
    void getPermission();
    void connectBikeServer_test();
    void quiteBikeClient();
    void sendRequestToGetAllBikeMsg();

    void addBikeSlot(int bikeId, double longitude, double latitude);
    void deleteBikeSlot(int bikeId);
    //void bike
    void openScanCodeWindow(int bikeId, double longitude, double latitude);
    void scanQRRequest(int eventId, int bikeId, double longitude, double latitude);
    void sendBroadcastRequest(int bikeId, int status, double longitude, double latitude);

private:
    static networkInterface *m_instance;     //它在内存中只占一份空间
    QTcpSocket* m_tcpClient;
    QTimer *m_tipTimer;
    int countTime;
    static QString m_userName;

    long m_rideStartTime;
    long m_rideEndTime;
    int  m_bikeId;
    long m_longitude;
    long m_latitude;

    char* m_sendMessage;
    char* m_readMessage;
    int m_readMsgLen;
    int m_sendMsgLen;
};

#endif // NETWORKINTERFACE_H
