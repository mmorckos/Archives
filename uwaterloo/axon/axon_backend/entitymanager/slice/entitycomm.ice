/*
 * entitycomm.ice
 *
 * ICE IDL definition for communications with entity managers.
 *
 * Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */
 
#ifndef ENTITYCOMM_ICE_
#define ENTITYCOMM_ICE_

#include <Ice/BuiltinSequences.ice>
#include <Glacier2/Session.ice>

#include <commutil.ice>

module EntityComm
{  
  exception InvalidMessageException
  {
    string reason;
  };
  
  exception CannotCreatePollSessionException
  {
    string reason;
  };

  ["ami"] interface EntityMonitorCallback
  {
    void newClientMsg (long timestamp, string clientId, string entityType, string msgType, \
      string msg);
    void newClientMsgDataSeq (long timestamp, string clientId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    void newClientMsgDataRec (long timestamp, string clientId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);
    void newUpdateMsg (long timestamp, string srcId, string srcType, string destType, \
      string msgType, string msg);
    void newEntityMsg (long timestamp, string srcEntityId, string destEntityId, string clientId, \
      string srcEntityType, string destEntityType, string clientType, string msgType, string msg);
    void newEntityMsgDataSeq (long timestamp, string srcEntityId, string destEntityId, \
      string clientId, string srcEntityType, string destEntityType, string clientType, \
      string msgType, CommUtil::DataSeq dataSeq);
    void newEntityMsgDataRec (long timestamp, string srcEntityId, string destEntityId, \
      string clientId, string srcEntityType, string destEntityType, string clientType, \
      string msgType, CommUtil::DataRecord dataRec);
    void stopSignal (long timestamp, string clientId, string entityType, string msg);
    void clearSignal (long timestamp, string clientId, string entityType, string msg);
    void resetSignal (long timestamp, string clientId, string entityType, string msg);
    void pong (long timestamp);
    
    void disconnect (long timestamp);
  };
  
  interface Session extends Glacier2::Session
  {
    void setCallback (EntityMonitorCallback *entityCb);
    ["ami"] long sendBasicPrfl (string entityType, string msg);
    ["ami"] long sendSvcPrflRecord (string entityType, int recordId, int count,
      CommUtil::DataRecord svcRecord);
    ["ami"] long sendToClient (string clientId, string entityType, string msgType, string msg);
    ["ami"] long sendToClientDataSeq (string clientId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    ["ami"] long sendToClientDataRec (string clientId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);
    ["ami"] long sendToEntity (string srcEntityId, string destEntityId, string clientId, \
      string srcEntityType, string destEntityType, string clientType, string msgType, string msg);
    ["ami"] long sendToEntityDataSeq (string srcEntityId, string destEntityId, string clientId, \
      string srcEntityType, string destEntityType, string clientType, string msgType, \
      CommUtil::DataSeq dataSeq);
    ["ami"] long sendToEntityDataRec (string srcEntityId, string destEntityId, string clientId, \
      string srcEntityType, string destEntityType, string clientType, string msgType, \
      CommUtil::DataRecord dataRec);
      
    ["ami"] long sendUpdate (string destId, string destType, string entityType, string msgType, \
      string msg);
      
    ["ami"] long sendUpdateDataSeq (string destId, string destType, string entityType,
      string msgType, CommUtil::DataSeq dataSeq);
    ["ami"] long ping ();
  };
  
/******************************************************/
/******************************************************/
  
  class EntityMonitorEvent
  {
    long timestamp;
  };

  sequence <EntityMonitorEvent> EntityMonitorEventSeq;
  
  class NewClientMsgEvent extends EntityMonitorEvent
  {
    string clientId;
    string entityType;
    string msgType;
    string msg;
  };
  
  class NewClientMsgDataSeqEvent extends EntityMonitorEvent
  {
    string clientId;
    string entityType;
    string msgType;
    CommUtil::DataSeq dataSeq;
  };
    
  class NewClientMsgDataRecEvent extends EntityMonitorEvent
  {
    string clientId;
    string entityType;
    string msgType;
    CommUtil::DataRecord dataRec;
  };
  
  class NewUpdateMsgEvent extends EntityMonitorEvent
  {
    string srcId;
    string srcType;
    string destType;
    string msgType;
    string msg;
  };
  
  class NewEntityMsgEvent extends EntityMonitorEvent
  {
    string srcEntityId;
    string destEntityId;
    string clientId;
    string srcEntityType;
    string destEntityType;
    string clientType;
    string msgType;
    string msg;
  };
  
  class NewEntityMsgDataSeqEvent extends EntityMonitorEvent
  {
    string srcEntityId;
    string destEntityId;
    string clientId;
    string srcEntityType;
    string destEntityType;
    string clientType;
    string msgType;
    CommUtil::DataSeq dataSeq;
  };
  
  class NewEntityMsgDataRecEvent extends EntityMonitorEvent
  {
    string srcEntityId;
    string destEntityId;
    string clientId;
    string srcEntityType;
    string destEntityType;
    string clientType;
    string msgType;
    CommUtil::DataRecord dataRec;
  };
  
  class StopEvent extends EntityMonitorEvent
  {
    string clientId;
    string entityType;
    string msg;
  };
  
  class ClearEvent extends EntityMonitorEvent
  {
    string clientId;
    string entityType;
    string msg;
  };
  
  class ResetEvent extends EntityMonitorEvent
  {
    string clientId;
    string entityType;
    string msg;
  };
  
  class PongEvent extends EntityMonitorEvent
  {};
  
  class DisconnectEvent extends EntityMonitorEvent
  {};
  
  ["ami"] interface PollSession
  {
    EntityMonitorEventSeq getUpdates ();
    long sendBasicPrfl (string entityType, string msg);
    long sendSvcPrflRecord (string entityType, int recordId, int count, \
      CommUtil::DataRecord svcRecord);
    long sendToClient (string clientId, string entityType, string msgType, string msg);
    long sendToClientDataSeq (string clientId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    long sendToClientDataRec (string clientId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);
    long sendToEntity (string srcEntityId, string destEntityId, string clientId, \
      string srcEntityType, string destEntityType, string clientType, string msgType, string msg);
    long sendToEntityDataSeq (string srcEntityId, string destEntityId, string clientId, \
      string srcEntityType, string destEntityType, string clientType, string msgType, \
      CommUtil::DataSeq dataSeq);  
    long sendToEntityDataRec (string srcEntityId, string destEntityId, string clientId, \
      string srcEntityType, string destEntityType, string clientType, string msgType, \
      CommUtil::DataRecord dataRec);
    
    long sendUpdate (string destId, string destType, string entityType, string msgType, \
      string msg);
      
    long sendUpdateDataSeq (string destId, string destType, string entityType,
      string msgType, CommUtil::DataSeq dataSeq);  
    long ping ();
    void destroy ();
  };
  
  interface PollSessionManager
  {
    ["ami"] PollSession* create (string msg) throws CannotCreatePollSessionException;
  };
};

#endif // ENTITYCOMM_ICE_
