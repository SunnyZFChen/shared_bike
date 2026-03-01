#include "bridge.h"
#include <qdebug.h>

bridge::bridge(networkInterface *interface, QObject *parent):QObject(parent),m_interface(interface)
{
    //connect(interface, &networkInterface::sendBikeMsg, this, &bridge::sendBikeMsg);
}

void bridge::sendRequestToGetAllBikeMsg(EventID_t id)
{

    emit m_interface->sendRequestToGetAllBikeMsgSlg(id);
}
