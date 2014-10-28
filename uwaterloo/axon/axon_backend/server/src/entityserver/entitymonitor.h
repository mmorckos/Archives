/*
 * entitymonitor.h
 *
 *  Created on: 2011-12-06
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef ENTITYMONITOR_H_
#define ENTITYMONITOR_H_

#include <map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "entitycomm.h"
#include "entitytopics.h"

class EntityMonitorCallbackAdapter : public IceUtil::Shared
{

public:
  virtual void newClientMsg (const EntityComm::NewClientMsgEventPtr& ev) = 0;
  virtual void newClientMsgDataSeq (const EntityComm::NewClientMsgDataSeqEventPtr& ev) = 0;
  virtual void newClientMsgDataRec (const EntityComm::NewClientMsgDataRecEventPtr& ev) = 0;
  virtual void newUpdateMsg (const EntityComm::NewUpdateMsgEventPtr& ev) = 0;
  virtual void newEntityMsg (const EntityComm::NewEntityMsgEventPtr& ev) = 0;
  virtual void newEntityMsgDataSeq (const EntityComm::NewEntityMsgDataSeqEventPtr& ev) = 0;
  virtual void newEntityMsgDataRec (const EntityComm::NewEntityMsgDataRecEventPtr& ev) = 0;
  virtual void stopSignal (const EntityComm::StopEventPtr& ev) = 0;
  virtual void clearSignal (const EntityComm::ClearEventPtr& ev) = 0;
  virtual void resetSignal (const EntityComm::ResetEventPtr& ev) = 0;
  virtual void pong (const EntityComm::PongEventPtr& ev) = 0;
  virtual void disconnect (const EntityComm::DisconnectEventPtr& ev) = 0;
};
typedef IceUtil::Handle <EntityMonitorCallbackAdapter> EntityMonitorCallbackAdapterPtr;

class EntityMonitor : public IceUtil::Shared
{

public:
  EntityMonitor (const EntityComm::EntityToFrontEndCommPrx& prx, const Ice::LoggerPtr& logger,
      const bool& flag);
  virtual ~EntityMonitor ();

  int checkRegRequest (const std::string& msg, std::string *entityId);
  void registerEntity (const std::string& entityId,
      const EntityMonitorCallbackAdapterPtr& callback);
  bool deregisterEntity (const std::string& entityId);

  Ice::Long updateBasicPrfl (const std::string& entityId, const std::string& entityType,
      const std::string& msg);
  Ice::Long relaySvcRecord (const std::string& entityId, const std::string& entityType,
      int recordId, int count, const CommUtil::DataRecord& svcRecord);
  Ice::Long sendToClient (const std::string& entityId, const std::string& clientId,
      const std::string& entityType, const std::string& msgType, const std::string& msg);
  Ice::Long sendToClientDataSeq (const std::string& entityId, const std::string& clientId,
      const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq);
  Ice::Long sendToClientDataRec (const std::string& entityId, const std::string& clientId,
      const std::string& entityType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec);
  Ice::Long sendToEntity (const std::string& srcEntityId, const std::string& destEntityId,
      const std::string& clientId, const std::string& srcEntityType,
      const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
      const std::string& msg);
  Ice::Long sendToEntityDataSeq (const std::string& srcEntityId, const std::string& destEntityId,
      const std::string& clientId, const std::string& srcEntityType,
      const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
      const CommUtil::DataSeq& dataSeq);
  Ice::Long sendToEntityDataRec (const std::string& srcEntityId, const std::string& destEntityId,
      const std::string& clientId, const std::string& srcEntityType,
      const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec);
  Ice::Long handleUpdate (const std::string& entityId, const std::string& destId,
      const std::string& destType, const std::string& entityType, const std::string& msgType,
      const std::string& msg);
  Ice::Long handleUpdateDataSeq (const std::string& entityId, const std::string& destId,
      const std::string& destType, const std::string& entityType, const std::string& msgType,
      const CommUtil::DataSeq& dataSeq);
  Ice::Long ping (const std::string& entityId);

  bool clientMsg (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType, const std::string& msg);
  bool clientMsgDataSeq (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq);
  bool clientMsgDataRec (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec);
  bool clientRequest (const std::string& clientId, const std::string& entityId,
      const std::string& entityType);
  bool clientUpdate (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msgType, const std::string& msg);
  bool relayStopSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg);
  bool relayClearSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg);
  bool relayResetSignal (const std::string& clientId, const std::string& entityId,
      const std::string& entityType, const std::string& msg);

protected:
  std::map <const std::string, EntityMonitorCallbackAdapterPtr> entityMonitorCallbackMap;
  /*!< Entity Id to callback map */
  std::map <const std::string, std::string> basicPrflMap; /*!< Entity Id to profiles map */
  std::map <const std::string, std::string> healthPrflMap;/*!< Entity Id health profile map */
  std::map <const std::string, std::string> statusMap;    /*!< Entity Id to status map */
  std::map <const std::string, std::vector <std::string> > associationMap;

  const EntityComm::EntityToFrontEndCommPrx entityToFrontEndComm_Prx;
  IceUtil::Mutex dataMutex;
  IceUtil::Mutex commMutex;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;               /*!< bool flag to toggle logging */

  void handleRequest (const std::string& msg, const std::string& msgType);
  bool exists (const std::string& entityId);
  bool extractMsg (const std::string& msg, std::string *prefix, std::string *body);
  bool extractInitMsg (const std::string& msg, std::string *prefix, std::string *id,
      std::string *body);
  void generalInfo ();
};
typedef IceUtil::Handle <EntityMonitor> EntityMonitorPtr;

#endif /* ENTITYMONITOR_H_ */
