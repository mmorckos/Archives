/*
 * session_i.h
 *
 *  Created on: 2011-12-08
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef SESSION_I_H_
#define SESSION_I_H_

#include "entitycomm.h"
#include "entitymonitor.h"

class Session_I : public EntityComm::Session
{

public:
  Session_I (const EntityMonitorPtr& em, const std::string& id, const Ice::LoggerPtr& logger,
      const bool flag);
  virtual ~Session_I ();

  virtual void setCallback (const EntityComm::EntityMonitorCallbackPrx& callback,
      const Ice::Current& curr);
  virtual Ice::Long sendBasicPrfl (const std::string& entityType, const std::string& msg,
      const Ice::Current& curr);
  virtual Ice::Long sendSvcPrflRecord (const std::string& entityType, int recordId, int count,
      const CommUtil::DataRecord& svcRecord, const Ice::Current& curr);
  virtual Ice::Long sendToClient (const std::string& clientId, const std::string& entityType,
      const std::string& msgType, const std::string& msg, const Ice::Current& curr);
  virtual Ice::Long sendToClientDataSeq (const std::string& clientId, const std::string& entityType,
      const std::string& msgType, const CommUtil::DataSeq& dataSeq, const Ice::Current& curr);
  virtual Ice::Long sendToClientDataRec (const std::string& clientId, const std::string& entityType,
      const std::string& msgType, const CommUtil::DataRecord& dataRec, const Ice::Current& curr);
  virtual Ice::Long sendToEntity (const std::string& srcEntityId, const std::string& destEntityId,
      const std::string& clientId, const std::string& srcEntityType,
      const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
      const std::string& msg, const Ice::Current& curr);
  virtual Ice::Long sendToEntityDataSeq (const std::string& srcEntityId,
      const std::string& destEntityId, const std::string& clientId,
      const std::string& srcEntityType, const std::string& destEntityType,
      const std::string& clientType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
      const Ice::Current& curr);
  virtual Ice::Long sendToEntityDataRec (const std::string& srcEntityId,
      const std::string& destEntityId, const std::string& clientId,
      const std::string& srcEntityType, const std::string& destEntityType,
      const std::string& clientType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec, const Ice::Current& curr);
  virtual Ice::Long sendUpdate (const std::string& destId, const std::string& destType,
      const std::string& entityType, const std::string& msgType, const std::string& msg,
      const Ice::Current& curr);
  virtual Ice::Long sendUpdateDataSeq (const std::string& destId, const std::string& destType,
      const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
      const Ice::Current& curr);
  virtual Ice::Long ping (const Ice::Current& curr);
  virtual void destroy (const Ice::Current& curr);

protected:
  const EntityMonitorPtr entityMonitor_Ptr;
  const std::string entityId;
  EntityMonitorCallbackAdapterPtr entityMonitor_Cb;
  IceUtil::Mutex mutex;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;
  bool terminated;

  void common ();
};
typedef IceUtil::Handle <Session_I> Session_IPtr;

#endif /* SESSION_I_H_ */
