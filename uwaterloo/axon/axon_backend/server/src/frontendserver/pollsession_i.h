/*
 * pollsession_i.h
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef POLLSESSION_I_H_
#define POLLSESSION_I_H_

#include "frontendcomm.h"
#include "clientmonitor.h"

class PollCallbackAdapter;
typedef IceUtil::Handle <PollCallbackAdapter> PollCallbackAdapterPtr;

class PollSession_I : public FrontEndComm::PollSession
{

public:
  PollSession_I (const ClientMonitorPtr& cm, const std::string& id,
      const Ice::LoggerPtr& logger, const bool flag);
  virtual ~PollSession_I ();

  virtual FrontEndComm::ClientMonitorEventSeq getUpdates (const Ice::Current& curr);
  virtual Ice::Long sendToEntity (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const std::string& msg, const Ice::Current& curr);
  virtual Ice::Long sendToEntityDataSeq (const std::string& entityId,
      const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
      const Ice::Current& curr);
  virtual Ice::Long sendToEntityDataRec (const std::string& entityId,
      const std::string& entityType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec, const Ice::Current& curr);
  virtual Ice::Long sendRequest (const std::string& destId, const std::string& destType,
      const std::string& msgType, const std::string& msg, const Ice::Current& curr);
  virtual Ice::Long sendStopSignal (const std::string& entityId, const std::string& entityType,
      const std::string& msg, const Ice::Current& curr);
  virtual Ice::Long sendClearSignal (const std::string& entityId, const std::string& entityType,
      const std::string& msg, const Ice::Current& curr);
  virtual Ice::Long sendResetSignal (const std::string& entityId, const std::string& entityType,
      const std::string& msg, const Ice::Current& curr);
  virtual Ice::Long ping (const Ice::Current& curr);
  virtual void destroy (const Ice::Current& curr);
  virtual IceUtil::Time timestamp () const;

protected:
  const ClientMonitorPtr clientMonitor_Ptr;
  const std::string clientId;
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
