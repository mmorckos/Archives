/*
 * clientmonitor.h
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef CLIENTMONITOR_H_
#define CLIENTMONITOR_H_

#include <map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "frontendcomm.h"
#include "frontendtopics.h"

const std::string GLOBALSVC_FILE = "../globalservices/global_services.xml";

class ClientMonitorCallbackAdapter : public IceUtil::Shared
{

public:
  virtual void newBasicPrflMsg (const FrontEndComm::NewBasicPrflMsgEventPtr& ev) = 0;
  virtual void newSvcRecordMsg (const FrontEndComm::NewSvcRecordMsgEventPtr& ev) = 0;
  virtual void newEntityMsg (const FrontEndComm::NewEntityMsgEventPtr& ev) = 0;
  virtual void newEntityMsgDataSeq (const FrontEndComm::NewEntityMsgDataSeqEventPtr& ev) = 0;
  virtual void newEntityMsgDataRec (const FrontEndComm::NewEntityMsgDataRecEventPtr& ev) = 0;
  virtual void newUpdateMsg (const FrontEndComm::NewUpdateMsgEventPtr& ev) = 0;
  virtual void newMsgDataSeq (const FrontEndComm::NewMsgDataSeqEventPtr& ev) = 0;
  virtual void pong (const FrontEndComm::PongEventPtr& ev) = 0;
  virtual void disconnect (const FrontEndComm::DisconnectEventPtr& ev) = 0;
};
typedef IceUtil::Handle <ClientMonitorCallbackAdapter> ClientMonitorCallbackAdapterPtr;

class ClientMonitor : public IceUtil::Shared
{

public:
  ClientMonitor (const FrontEndComm::FrontEndToEntityCommPrx& prx,
      const Ice::LoggerPtr& logger, const bool& flag);
  virtual ~ClientMonitor ();

  int checkRegRequest (const std::string& msg, std::string *clientId);
  void registerClient (const std::string& clientId,
      const ClientMonitorCallbackAdapterPtr& callback);
  bool deregisterClient (const std::string& clientId);
  Ice::Long sendToEntity (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType, std::string msg);
  Ice::Long sendToEntityDataSeq (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq);
  Ice::Long sendToEntityDataRec (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec);
  Ice::Long handleRequest (const std::string& clientId, const std::string& destId,
      const std::string& destType, const std::string& msgType, const std::string& msg);
  Ice::Long relayStopSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg);
  Ice::Long relayClearSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg);
  Ice::Long relayResetSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg);
  Ice::Long ping (const std::string& clientId);

  void entityBasicPrflUpdate (const std::string& entityId, const std::string& entityType,
      const std::string& msg);
  void entitySvcRecord (const std::string& entityId, const std::string& entityType,
      int recordId, int count, const CommUtil::DataRecord& svcRecord);
  void entityMsg (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const std::string& msg);
  void entityMsgDataSeq (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const CommUtil::DataSeq& dataSeq);
  void entityMsgDataRec (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const CommUtil::DataRecord& dataRec);
  void entityUpdate (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const std::string& msg);

protected:
  std::map <const std::string, ClientMonitorCallbackAdapterPtr> clientMonitorCallbackMap;
  /*!< Client ID-to-callback map */
  const FrontEndComm::FrontEndToEntityCommPrx frontEndToEntityComm_Prx;
  std::map <const std::string, std::string> entityBasicPrflMap; /*!< Entity Id to profiles map */
  std::map <const std::string, std::string> entityToClientMap;  /*!< Entity to Client Map */
  CommUtil::DataSeq globalServicesData;
  IceUtil::Mutex dataMutex;             /*!< Mutex for data access protection */
  const Ice::LoggerPtr iceLogger_Ptr;   /*!< Pointer to the ICE logging facility */
  const bool logFlag;                   /*!< bool flag to toggle logging */

  bool exists (const std::string& entityId);
  bool extractInitMsg (const std::string& msg, std::string *prefix, std::string *id,
      std::string *body);
  void loadGlobalServices ();
  void generalInfo ();
};
typedef IceUtil::Handle <ClientMonitor> ClientMonitorPtr;

#endif /* CLIENTMONITOR_H_ */
