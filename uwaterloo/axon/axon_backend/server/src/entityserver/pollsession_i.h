/*
 * pollsession_i.h
 *
 *  Created on: 2011-12-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef POLLSESSION_I_H_
#define POLLSESSION_I_H_

#include "entitycomm.h"
#include "entitymonitor.h"

class PollCallbackAdapter;
typedef IceUtil::Handle <PollCallbackAdapter> PollCallbackAdapterPtr;

class PollSession_I : public EntityComm::PollSession
{

public:
  PollSession_I (const EntityMonitorPtr& em, const std::string& id, const Ice::LoggerPtr& logger,
      const bool flag);
  virtual ~PollSession_I ();

  virtual EntityComm::EntityMonitorEventSeq getUpdates (const Ice::Current& curr);

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
  virtual IceUtil::Time timestamp () const;

protected:
  const EntityMonitorPtr entityMonitor_Ptr;
  const std::string entityId;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;
  const PollCallbackAdapterPtr pollCallBackAdapter_Ptr;
  IceUtil::Time currTimeStamp;
  IceUtil::Mutex mutex;
  bool terminated;

  void common ();
};
typedef IceUtil::Handle <PollSession_I> PollSession_IPtr;

#endif /* POLLSESSION_I_H_ */
