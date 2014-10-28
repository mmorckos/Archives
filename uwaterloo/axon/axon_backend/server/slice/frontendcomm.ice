/*
 * frontendcomm.ice
 *
 * ICE IDL definition for communications with client applications.
 *
 * Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef FRONTENDCOMM_ICE_
#define FRONTENDCOMM_ICE_

#include <Ice/BuiltinSequences.ice>
#include <Glacier2/Session.ice>

#include <commutil.ice>

module FrontEndComm
{  
  exception InvalidMessageException
  {
    string reason;
  };
  
  exception CannotCreatePollSessionException
  {
    string reason;
  };
  
  ["ami"] interface ClientMonitorCallback
  {
    void newBasicPrflMsg (long timestamp, string entityId, string entityType, string msg);
    void newSvcRecordMsg (long timestamp, string entityId, string entityType, int recordId, \
    int count, CommUtil::DataRecord svcRecord);
    void newEntityMsg (long timestamp, string entityId, string entityType, string msgType, \
      string msg);
    void newEntityMsgDataSeq (long timestamp, string entityId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    void newEntityMsgDataRec (long timestamp, string entityId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);
    void newUpdateMsg (long timestamp, string sourceId, string sourceType, string msgType, \
    string msg);
    void newMsgDataSeq (long timestamp, string sourceId, string sourceType, string msgType, \
      CommUtil::DataSeq dataSeq);
    void pong (long timestamp);
    void disconnect (long timestamp);
  };
  
  interface Session extends Glacier2::Session
  {
    void setCallback (ClientMonitorCallback *clientCb);
    ["ami"] long sendToEntity (string entityId, string entityType, string msgType, string msg);
    ["ami"] long sendToEntityDataSeq (string entityId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    ["ami"] long sendToEntityDataRec (string entityId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);  
    ["ami"] long sendRequest (string destId, string destType, string msgType, string msg);
    ["ami"] long sendStopSignal (string entityId, string entityType, string msg);
    ["ami"] long sendClearSignal (string entityId, string entityType, string msg);
    ["ami"] long sendResetSignal (string entityId, string entityType, string msg);
    ["ami"] long ping ();
  };
  
/******************************************************/
/******************************************************/

  class ClientMonitorEvent
  {
    long timestamp;
  };
  
  sequence <ClientMonitorEvent> ClientMonitorEventSeq;

  class NewBasicPrflMsgEvent extends ClientMonitorEvent
  {
    string entityId;
    string entityType;
    string msg;
  };
  
  class NewSvcRecordMsgEvent extends ClientMonitorEvent
  {
    string entityId;
    string entityType; 
    int recordId;
    int count;
    CommUtil::DataRecord svcRecord;
  };

  class NewEntityMsgEvent extends ClientMonitorEvent
  {
    string entityId;
    string entityType;
    string msgType;
    string msg;
  };
  
  class NewEntityMsgDataSeqEvent extends ClientMonitorEvent
  {
    string entityId;
    string entityType;
    string msgType;
    CommUtil::DataSeq dataSeq;
  };
  
  class NewEntityMsgDataRecEvent extends ClientMonitorEvent
  {
    string entityId;
    string entityType;
    string msgType;
    CommUtil::DataRecord dataRec;
  };
  
  class NewUpdateMsgEvent extends ClientMonitorEvent
  {
    string sourceId;    
    string sourceType;
    string msgType;      
    string msg;
  };
  
  class NewMsgDataSeqEvent extends ClientMonitorEvent
  {
    string sourceId;    
    string sourceType;
    string msgType;      
    CommUtil::DataSeq dataSeq;
  };
  
  class PongEvent extends ClientMonitorEvent
  {};
  
  class DisconnectEvent extends ClientMonitorEvent
  {};
  
  ["ami"] interface PollSession
  {
    ClientMonitorEventSeq getUpdates ();
    long sendToEntity (string entityId, string entityType, string msgType, string msg);
    long sendToEntityDataSeq (string entityId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    long sendToEntityDataRec (string entityId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);
    long sendRequest (string destId, string destType, string msgType, string msg);
    long sendStopSignal (string entityId, string entityType, string msg);
    long sendClearSignal (string entityId, string entityType, string msg);
    long sendResetSignal (string entityId, string entityType, string msg);
    long ping ();
    void destroy ();
  };
  
  interface PollSessionManager
  {
    ["ami"] PollSession* create (string msg) throws CannotCreatePollSessionException;
  };
};

#endif // FRONTENDCOMM_ICE_
