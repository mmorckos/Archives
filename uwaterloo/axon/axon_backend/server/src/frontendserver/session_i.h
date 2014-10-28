/*
 * session_i.h
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef SESSION_I_H_
#define SESSION_I_H_

#include "frontendcomm.h"
#include "clientmonitor.h"

class Session_I : public FrontEndComm::Session
{

public:
  Session_I (const ClientMonitorPtr& cm, const std::string& id, const Ice::LoggerPtr& logger,
      const bool flag);
  virtual ~Session_I ();

  virtual void setCallback (const FrontEndComm::ClientMonitorCallbackPrx& callback,
      const Ice::Current& curr);
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

protected:
  const ClientMonitorPtr clientMonitor_Ptr;
  const std::string clientId;
  ClientMonitorCallbackAdapterPtr clientMonitor_Cb;
  IceUtil::Mutex mutex;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;
  bool terminated;

  void common ();
};
typedef IceUtil::Handle <Session_I> Session_IPtr;

#endif /* SESSION_I_H_ */
