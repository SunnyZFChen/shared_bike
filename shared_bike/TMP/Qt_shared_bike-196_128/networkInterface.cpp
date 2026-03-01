#include "networkInterface.h"
#include "bike.pb.h"
#include <QDateTime>
#include <QDebug>
networkInterface *networkInterface::m_instance = nullptr;
QString networkInterface::m_userName =  "123456789";

networkInterface::networkInterface(QObject *parent):QObject(parent)
{
    registerNatives();
    initMyNumber();
}

void networkInterface::startScanQRCode(QString name)
{
    #ifdef ANDROID
    QAndroidJniObject javaAction =
    QAndroidJniObject::fromString(name);
    QAndroidJniObject::callStaticMethod<void> (
            "an/qt/qtWithJava/MyJavaClass",
            "scanQRCode", "(Ljava/lang/String;)V",
            javaAction.object<jstring>()
    );

#endif
}

//注册java类，便于java中调用Qt的方法
void networkInterface::registerNatives()
{
    JNINativeMethod methods[] = {"SendDataToQt", "(Ljava/lang/String;)V", (void*)sendDataToQt};

    QAndroidJniEnvironment env;
    const char* mClassName ="an/qt/qtWithJava/MyJavaClass";
    jclass j_class;
    j_class = env->FindClass(mClassName);
    if (j_class == nullptr)
    {
        qDebug()<<"erro j_class";
        return ;
    }
    jint mj = env->RegisterNatives(j_class, methods, sizeof(methods) / sizeof(methods[0]));
    if(mj != JNI_OK) qDebug()<<"env->RegisterNatives false!";
    if(env->ExceptionCheck()) env->ExceptionClear();
}

void networkInterface::sendDataToQt(JNIEnv *env, jobject type, jstring jdata)
{

    const char *cdata  = env->GetStringUTFChars(jdata, 0);
    QString qdata(cdata);

    QStringList dataSplit = qdata.split("-");
    qDebug()<<"dataSplit.at(0):"<< dataSplit.at(0)<<"dataSplit.at(1):"<< dataSplit.at(1);
    if(dataSplit.at(0)=="#bike" && dataSplit.at(1)==QString::number(m_instance->m_bikeId))
    {
        m_instance->scanQRRequest(EEVENTID_SCANQRSTART_REQ, m_instance->m_bikeId, m_instance->m_longitude, m_instance->m_latitude);
        //emit m_instance->isSuccess(true);
    }
    else if(dataSplit.at(0)=="#bike"){
        emit m_instance->msgTipPrintToHtmlSig("您扫的码不是这台奇牛单车的二维码哦！");
    }
    else{
        emit m_instance->msgTipPrintToHtmlSig("您扫的码不是奇牛单车的二维码哦！");
    }


}

networkInterface *networkInterface::getInstance()
{
    if(m_instance==nullptr)
    {
        m_instance = new networkInterface();
    }
    return m_instance;
}

void networkInterface::initMyNumber()
{
    //m_tipLabel = new QLabel(this);
    m_tipTimer = new QTimer(this);
    m_latitude = 0;
    m_longitude = 0;
    m_sendMsgLen = 0;
    m_readMsgLen = 0;
    m_rideStartTime = 0;
    countTime = 0;
    m_rideEndTime = 0;
    m_sendMessage = nullptr;
    m_readMessage = nullptr;
    m_tcpClient = nullptr;
}

void networkInterface::connectServer()
{
    if(!m_tcpClient)
    {
        m_tcpClient = new QTcpSocket();   //实例化
    }
    m_tcpClient->abort();                 //取消原有连接

    m_tcpClient->connectToHost(shareBikeServer_IP,shareBikeServer_port); //IP以及端口号

    if (m_tcpClient->waitForConnected(2000))  // 连接成功
    {
        emit msgTipPrintToHtmlSig("已连接到共享单车服务器！");

        connect(m_tcpClient, &QTcpSocket::readyRead, this, &networkInterface::ReadData);
        //connect(m_tcpClient, &QTcpSocket::disconnectFromHost, this, &networkInterface::disconnectSer);
        connect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(ReadError(QAbstractSocket::SocketError)));
        emit ServerConnectSuccessSig();
        //login();

    }else{
        //emit msgTipPrintToHtmlSig("无法连接到共享单车服务器！");
        delete m_tcpClient;
        m_tcpClient = nullptr;
        emit ServerConnectFalseSig("无法连接到共享单车服务器！");
    }
}


void networkInterface::setTcpClient(QTcpSocket *tcpclient)
{
    if(!tcpclient)
    {
        return;
    }

    qDebug()<<"tcpclient"<<tcpclient;
    m_tcpClient  = tcpclient;
    qDebug()<<"setTcpClient(QTcpSocket *tcpclient)this->m_tcpClient:"<<this->m_tcpClient;
    connect(m_tcpClient, &QTcpSocket::readyRead, this, &networkInterface::ReadData);
    //connect(m_tcpClient, &QTcpSocket::disconnectFromHost, this, &networkInterface::disconnectSer);
    //connect(m_tcpClient, &QTcpSocket::disconnected, this, &networkInterface::ServerReadError);
    connect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(ServerReadError(QAbstractSocket::SocketError)));

}

void networkInterface::sendRequestToGetAllBikeMsg()
{
    if(!m_sendMessage) m_sendMessage = new char[1024];
    tutorial::list_all_bikeinfo_request labr;

    std::string userName = m_userName.toStdString();
    labr.set_username(userName.c_str());
    m_sendMsgLen = labr.ByteSizeLong();
    memcpy(m_sendMessage, "FBEB", 4);

    *(u16*)(m_sendMessage + 4) = EEVENTID_LIST_BIKEINFO_REQ;//
    *(i32*)(m_sendMessage + 6) = m_sendMsgLen;

    labr.SerializeToArray(m_sendMessage+10, m_sendMsgLen);

    if(m_tcpClient->write(m_sendMessage, m_sendMsgLen+10)==-1)
    {
        emit msgTipPrintToHtmlSig("单车信息同步请求失败！");
    }

    m_tcpClient->flush();
    m_sendMsgLen = 0;
    delete[] m_sendMessage;
    m_sendMessage = nullptr;
}

void networkInterface::addBikeSlot(int bikeId, double longitude, double latitude)
{
    scanQRRequest(EEVENTID_ADDBIKE_REQ, bikeId, longitude, latitude);
}

void networkInterface::deleteBikeSlot(int bikeId)
{
    scanQRRequest(EEVENTID_DELETEBIKE_REQ, bikeId, 0, 0);
}

void networkInterface::openScanCodeWindow(int bikeId, double longitude, double latitude)
{
    m_bikeId = bikeId;
    m_longitude = longitude;
    m_latitude = latitude;
    startScanQRCode("MediaStore.ACTION_IMAGE_CAPTURE");
}

void networkInterface::scanQRRequest(int eventId, int bikeId, double longitude, double latitude)
{

    if(!m_sendMessage) m_sendMessage = new char[1024];
    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    int timeT = time.toTime_t();   //将当前时间转为时间戳
    std::string userName = m_userName.toStdString();
    switch (eventId)
    {
        case EEVENTID_SCANQRSTART_REQ:
        {

            tutorial::bike_scanQR_start_request bssr;
            bssr.set_username(userName.c_str());
            bssr.set_bikeid(bikeId);
            bssr.set_longitude(longitude);
            bssr.set_latitude(latitude);
            bssr.set_timestamp(timeT);
            m_sendMsgLen = bssr.ByteSizeLong();
            memcpy(m_sendMessage, "FBEB", 4);
            *(u16*)(m_sendMessage + 4) = eventId;//EEVENTID_SCANQRSTART_REQ
            *(i32*)(m_sendMessage + 6) = m_sendMsgLen;
            bssr.SerializeToArray(m_sendMessage+10, m_sendMsgLen);
            break;
        }
        case EEVENTID_SCANQREND_REQ:
        {
            //用户正在扫码还车EEVENTID_SCANQREND_REQ;
            tutorial::bike_scanQR_end_request bser;
            bser.set_username(userName.c_str());
            bser.set_bikeid(bikeId);
            bser.set_longitude(longitude);
            bser.set_latitude(latitude);
            bser.set_timestamp(timeT);
            m_sendMsgLen = bser.ByteSizeLong();

            memcpy(m_sendMessage, "FBEB", 4);
            *(u16*)(m_sendMessage + 4) = eventId;//EEVENTID_GET_MOBLIE_CODE_REQ
            *(i32*)(m_sendMessage + 6) = m_sendMsgLen;
            bser.SerializeToArray(m_sendMessage+10, m_sendMsgLen);
            break;
        }
        case EEVENTID_FAULT_REQ:
        {
            tutorial::bike_fault_request bfr;
            bfr.set_username(userName.c_str());
            bfr.set_bike_id(bikeId);
            bfr.set_longitude(longitude);
            bfr.set_latitude(latitude);
            bfr.set_timestamp(timeT);
            m_sendMsgLen = bfr.ByteSizeLong();

            memcpy(m_sendMessage, "FBEB", 4);

            *(u16*)(m_sendMessage + 4) = eventId;//
            printf("*(u16*)(msg + 4)=%d\n", *(u16*)(m_sendMessage + 4));
            *(i32*)(m_sendMessage + 6) = m_sendMsgLen;
            printf("*(i32*)(msg + 6)=%d\n", *(i32*)(m_sendMessage + 6));
            bfr.SerializeToArray(m_sendMessage+10, m_sendMsgLen);
            break;
        }

        case EEVENTID_REPAIRFINISH_REQ:
        {
            tutorial::bike_repair_finish_request brfr;
            brfr.set_username(userName.c_str());
            brfr.set_bike_id(bikeId);
            brfr.set_longitude(longitude);
            brfr.set_latitude(latitude);
            brfr.set_timestamp(timeT);
            m_sendMsgLen = brfr.ByteSizeLong();

            memcpy(m_sendMessage, "FBEB", 4);

            *(u16*)(m_sendMessage + 4) = eventId;//
            printf("*(u16*)(msg + 4)=%d\n", *(u16*)(m_sendMessage + 4));
            *(i32*)(m_sendMessage + 6) = m_sendMsgLen;
            printf("*(i32*)(msg + 6)=%d\n", *(i32*)(m_sendMessage + 6));
            brfr.SerializeToArray(m_sendMessage+10, m_sendMsgLen);
            break;
        }
        case EEVENTID_ADDBIKE_REQ:
        {
            tutorial::addBike_request ar;
            ar.set_bikeid(bikeId);
            ar.set_longitude(longitude);
            ar.set_latitude(latitude);
            m_sendMsgLen = ar.ByteSizeLong();
            memcpy(m_sendMessage, "FBEB", 4);

            *(u16*)(m_sendMessage + 4) = eventId;//
            *(i32*)(m_sendMessage + 6) = m_sendMsgLen;
            ar.SerializeToArray(m_sendMessage+10, m_sendMsgLen);
            break;
        }
        case EEVENTID_DELETEBIKE_REQ:
        {
            tutorial::deleteBike_request dr;
            dr.set_bikeid(bikeId);

            m_sendMsgLen = dr.ByteSizeLong();
            memcpy(m_sendMessage, "FBEB", 4);

            *(u16*)(m_sendMessage + 4) = eventId;//
            *(i32*)(m_sendMessage + 6) = m_sendMsgLen;
            dr.SerializeToArray(m_sendMessage+10, m_sendMsgLen);
            break;
        }
        default:
            return;
    }

    if(m_tcpClient->write(m_sendMessage, m_sendMsgLen+10)!=-1)
    {
        qDebug()<< "请求[ "<< eventId << "] success!";
    }

    m_tcpClient->flush();
    m_sendMsgLen = 0;
    delete[] m_sendMessage;
    m_sendMessage = nullptr;

}

void networkInterface::ReadData()
{
    m_readMessage = new char[1024];
    int readLen = m_tcpClient->read(m_readMessage, 1024);
    m_readMessage[readLen] = '\0';

    qDebug()<<"ReadData->readLen:"<<readLen;
    if(readLen != -1)
    {
         if(strncmp(m_readMessage, "FBEB", 4) ==0)
         {
             u16 id = *(u16*)(m_readMessage + 4);
             m_readMsgLen = *(i32*)(m_readMessage + 6);
             ReadMessageFromServer(id);
         }
         else
         {
            emit msgTipPrintToHtmlSig("校验信息错误（!='FBEB')");
         }
    }
}

void networkInterface::ReadMessageFromServer(u32 id)
{

    switch (id)
    {
        //添加单车响应

        case EEVENTID_ADDBIKE_RSP:
        {
            tutorial::addBike_response ar;
            ar.ParseFromArray(m_readMessage+10, m_readMsgLen);
            if(ar.rescode() == ERRC_SUCCESS)
            {
                int bikeId = ar.bikeid();
                emit addBikeResSig(bikeId, ar.longitude(), ar.latitude());
                //sendBroadcastRequest(m_bikeBtn->getBikeId(), BIKE_RIDE_START, 99999, 99999);
            }
            else
            {
                emit msgTipPrintToHtmlSig(QString("添加单车请求失败!错误码：%1").arg(ar.rescode()));
            }
            break;
        }
        //删除单车响应
        case EEVENTID_DELETEBIKE_RSP:
        {
            tutorial::deleteBike_response dr;
            dr.ParseFromArray(m_readMessage+10, m_readMsgLen);
            if(dr.rescode() == ERRC_SUCCESS)
            {
                emit deleteBikeResSig(dr.bikeid());
                //sendBroadcastRequest(m_bikeBtn->getBikeId(), BIKE_RIDE_START, 99999, 99999);
            }
            else
            {
                emit msgTipPrintToHtmlSig(QString("删除单车请求失败!错误码：%1").arg(dr.rescode()));
            }
            break;
        }
        //单车扫码骑行相应
        case EEVENTID_SCANQRSTART_PSP:
        {
            tutorial::bike_scanQR_start_response bssr;
            bssr.ParseFromArray(m_readMessage+10, m_readMsgLen);
            if(bssr.rescode() == ERRC_SUCCESS)
            {
                m_rideStartTime = bssr.timestamp();
                emit sendBikeStartResSig();
                //sendBroadcastRequest(m_bikeBtn->getBikeId(), BIKE_RIDE_START, 99999, 99999);
            }
            else
            {
                emit msgTipPrintToHtmlSig(QString("扫码请求骑行失败!错误码：%1").arg(bssr.rescode()));
            }
            break;
        }
        //扫描还车响应
        case EEVENTID_SCANQREND_PSP:
        {
            tutorial::bike_scanQR_end_response bser;
            bser.ParseFromArray(m_readMessage+10, m_readMsgLen);
            if(bser.rescode() == ERRC_SUCCESS)
            {
                m_rideEndTime = bser.timestamp();
                emit sendBikeEndResSig();
            }
            else
            {

                emit msgTipPrintToHtmlSig(QString("扫码还车请求失败!错误码：%1").arg(bser.rescode()));
            }
            break;
        }
        //扫码故障报修响应
        case EEVENTID_FAULT_PSP:
        {
            tutorial::bike_fault_response bfr;
            bfr.ParseFromArray(m_readMessage+10, m_readMsgLen);
            qDebug()<<"bser.code():"<<bfr.rescode();
            if(bfr.rescode() == ERRC_SUCCESS)
            {
                //sendBroadcastRequest(m_bikeBtn->getBikeId(), BIKE_FAULT, 99999, 99999);
            }
            break;
        }
        //扫码报修完成响应
        case EEVENTID_REPAIRFINISH_PSP:
        {
            tutorial::bike_repair_finish_response brfr;
            brfr.ParseFromArray(m_readMessage+10, m_readMsgLen);
            qDebug()<<"brfr.code():"<<brfr.rescode();
            if(brfr.rescode() == ERRC_SUCCESS)
            {
                //sendBroadcastRequest(m_bikeBtn->getBikeId(), BIKE_REPAIR_FINISH, 99999, 99999);
            }
            break;
        }
        //广播响应
        case EEVENTID_BROADCAST_MSG_RSP:
        {
            tutorial::broadcast_msg_response bmr;
            bmr.ParseFromArray(m_readMessage+10, m_readMsgLen);
            emit bikeInfoBroadcastResSig(bmr.bikeid(), bmr.bikestatus(), bmr.longitude(), bmr.latitude());
            break;
        }
        //获取所有单车信息响应
        case EEVENTID_LIST_BIKEINFO_RSP:
        {
            tutorial::list_all_bikeinfo_response labr;

            labr.ParseFromArray(m_readMessage+10, m_readMsgLen);
            if(labr.rescode()!=ERRC_SUCCESS)
            {
                emit msgTipPrintToHtmlSig("请求错误, 单车信息同步失败！");
                return;
            }
            int bikeNum = labr.records_size();
            qDebug()<<"bikeNum: "<<bikeNum;
            for(int i=0; i<bikeNum; i++)
            {
                const tutorial::list_all_bikeinfo_response_bikeinfo &bikeinfo = labr.records(i);

                emit ListAllBikeMsgSig(bikeinfo.bikeid(), bikeinfo.bikestatus(), bikeinfo.longitude(), bikeinfo.latitude());
            }
            break;
        }
        default:
            return;
    }
}

void networkInterface::sendBroadcastRequest(int bikeId, int status, double longitude, double latitude)
{
    tutorial::broadcast_msg_request bmr;

    if(!m_sendMessage) m_sendMessage = new char[1024];
    //设置单车ID及当前状态
    bmr.set_bikeid(bikeId);
    bmr.set_bikestatus(status);
    bmr.set_longitude(longitude);
    bmr.set_latitude(latitude);

    int len = bmr.ByteSizeLong();
    memcpy(m_sendMessage, "FBEB", 4);

    *(u16*)(m_sendMessage + 4) = EEVENTID_BROADCAST_MSG_REQ;//EEVENTID_BROADCAST_MSG_REQ
    *(i32*)(m_sendMessage + 6) = len;
    bmr.SerializeToArray(m_sendMessage+10, len);

    if(m_tcpClient->write(m_sendMessage, len+10)!=-1)
    {
        qDebug()<<"bike_scanQR_start_request success!";
    }
    m_tcpClient->flush();
    delete[] m_sendMessage;
    m_sendMessage = nullptr;
}

void networkInterface::ReadError(QAbstractSocket::SocketError)
{
    emit msgTipPrintToHtmlSig("与服务器连接已断开！");
    //emit ServerConnectFalseSig("与服务器断开连接！");
    if(m_tcpClient)
    {
        m_tcpClient->disconnectFromHost();
        delete  m_tcpClient;
        m_tcpClient = nullptr;
    }
    //UserLogin::getInstance()->openQmlLoginPage();
}

void networkInterface::connectBikeServer()
{
    //UserLogin::getInstance()->openQmlLoginPage();
    m_tcpClient = UserLogin::getInstance()->getTcpClient();
    m_userName = UserLogin::getInstance()->getUserName();
    //UserLogin::getInstance()->distroyFreeMemory();
    qDebug()<<"m_tcpClient: %p"<<m_tcpClient;
    if(m_tcpClient)
    {
        connect(m_tcpClient, &QTcpSocket::readyRead, this, &networkInterface::ReadData);
        //connect(m_tcpClient, &QTcpSocket::disconnectFromHost, this, &networkInterface::disconnectSer);
        connect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(ReadError(QAbstractSocket::SocketError)));
        emit ServerConnectSuccessSig();
    }
    else{
        connectServer();
    }
}

void networkInterface::getPermission()
{
    #ifdef ANDROID
    //QAndroidJniObject javaAction =
    //QAndroidJniObject::fromString(name) ;
    QAndroidJniObject::callStaticMethod<void> (
            "an/qt/qtWithJava/MyJavaClass",
            "getPermission", "()V"
    );

    #endif
}

void networkInterface::connectBikeServer_test()
{
    connectServer();
}

void networkInterface::quiteBikeClient()
{
    UserLogin::openQmlLoginPage();
}


