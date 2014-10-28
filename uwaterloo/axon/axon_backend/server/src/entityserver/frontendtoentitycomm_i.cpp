/*
 * frontendtoentitycomm_i.cpp
 *
 *  Created on: 2012-01-11
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "frontendtoentitycomm_i.h"

FrontEndToEntityComm_I::FrontEndToEntityComm_I (const EntityMonitorPtr& em) :
  entityMonitor_Ptr (em)
{}

void
FrontEndToEntityComm_I::clientMsg (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType,
    const std::string& msg, const Ice::Current& curr)
{
  entityMonitor_Ptr->clientMsg (clientId, entityId, entityType, msgType, msg);
}

void
FrontEndToEntityComm_I::clientMsgDataSeq (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType,
    const CommUtil::DataSeq& dataSeq, const Ice::Current& curr)
{
  entityMonitor_Ptr->clientMsgDataSeq (clientId, entityId, entityType, msgType, dataSeq);
}

void
FrontEndToEntityComm_I::clientMsgDataRec (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType,
    const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  entityMonitor_Ptr->clientMsgDataRec (clientId, entityId, entityType, msgType, dataRec);
}

void
FrontEndToEntityComm_I::clientRequest (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const Ice::Current& curr)
{
  entityMonitor_Ptr->clientRequest (clientId, entityId, entityType);
}

void
FrontEndToEntityComm_I::clientUpdate (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const std::string& msg,
    const Ice::Current& curr)
{
  entityMonitor_Ptr->clientUpdate (clientId, entityId, entityType, msgType, msg);
}

void
FrontEndToEntityComm_I::stopSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg, const Ice::Current& curr)
{
  entityMonitor_Ptr->relayStopSignal (clientId, entityId, entityType, msg);
}

void
FrontEndToEntityComm_I::clearSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg, const Ice::Current& curr)
{
  entityMonitor_Ptr->relayClearSignal (clientId, entityId, entityType, msg);
}

void
FrontEndToEntityComm_I::resetSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg, const Ice::Current& curr)
{
  entityMonitor_Ptr->relayResetSignal (clientId, entityId, entityType, msg);
}
