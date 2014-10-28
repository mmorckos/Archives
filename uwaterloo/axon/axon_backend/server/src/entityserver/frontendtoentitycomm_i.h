/*
 * frontendtoentitycomm_i.h
 *
 *  Created on: 2012-01-11
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef FRONTENDTOENTITYCOMM_I_H_
#define FRONTENDTOENTITYCOMM_I_H_

#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include "entitymonitor.h"
#include "frontendtopics.h"

class FrontEndToEntityComm_I : virtual public FrontEndComm::FrontEndToEntityComm
{

public:
  FrontEndToEntityComm_I (const EntityMonitorPtr& em);

  virtual void clientMsg (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType,
      const std::string& msg, const Ice::Current& curr);
  virtual void clientMsgDataSeq (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType,
      const CommUtil::DataSeq& dataSeq, const Ice::Current& curr);
  virtual void clientMsgDataRec (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec, const Ice::Current& curr);
  virtual void clientRequest (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const Ice::Current& curr);
  virtual void clientUpdate (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType, const std::string& msg,
      const Ice::Current& curr);
  virtual void stopSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg, const Ice::Current& curr);
  virtual void clearSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg, const Ice::Current& curr);
  virtual void resetSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg, const Ice::Current& curr);

protected:
  const EntityMonitorPtr entityMonitor_Ptr;
};

#endif /* FRONTENDTOENTITYCOMM_I_H_ */
