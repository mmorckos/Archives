/*
 * entitytofrontendcomm_i.cpp
 *
 *  Created on: 2012-01-10
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "entitytofrontendcomm_i.h"

EntityToFrontEndComm_I::EntityToFrontEndComm_I (const ClientMonitorPtr& cm) :
  clientMonitor_Ptr (cm)
{}

void
EntityToFrontEndComm_I::entityBasicPrflUpdate (const std::string& entityId,
    const std::string& entityType, const std::string& msg, const Ice::Current& curr)
{
  clientMonitor_Ptr->entityBasicPrflUpdate (entityId, entityType, msg);
}

void
EntityToFrontEndComm_I::entitySvcRecord (const std::string& entityId, const std::string& entityType,
    int recordId, int count, const CommUtil::DataRecord& svcRecord, const Ice::Current& curr)
{
  clientMonitor_Ptr->entitySvcRecord (entityId, entityType, recordId, count, svcRecord);
}

void
EntityToFrontEndComm_I::entityMsg (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  clientMonitor_Ptr->entityMsg (entityId, entityType, msgType, msg);
}

void
EntityToFrontEndComm_I::entityMsgDataSeq (const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
    const Ice::Current& curr)
{
  clientMonitor_Ptr->entityMsgDataSeq (entityId, entityType, msgType, dataSeq);
}

void
EntityToFrontEndComm_I::entityMsgDataRec (const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataRecord& dataRec,
    const Ice::Current& curr)
{
  clientMonitor_Ptr->entityMsgDataRec (entityId, entityType, msgType, dataRec);
}

void
EntityToFrontEndComm_I::entityUpdate (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  clientMonitor_Ptr->entityUpdate (entityId, entityType, msgType, msg);
}
