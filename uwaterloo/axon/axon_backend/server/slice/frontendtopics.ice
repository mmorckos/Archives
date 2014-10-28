/*
 * frontendtopics.ice
 *
 * ICE IDL definition for inter-server communications.
 *
 * Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */
 
#ifndef FRONTENDTOPICS_ICE_
#define FRONTENDTOPICS_ICE_

#include <commutil.ice>

module FrontEndComm
{
  interface FrontEndToEntityComm
  {
    void clientMsg (string clientId, string entityId, string entityType, string msgType, \
      string msg);
    void clientMsgDataSeq (string clientId, string entityId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    void clientMsgDataRec (string clientId, string entityId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);
    void clientRequest (string clientId, string entityId, string entityType);
    void clientUpdate (string clientId, string entityId, string entityType, string msgType, \
      string msg);
    void stopSignal (string clientId, string entityId, string entityType, string msg);
    void clearSignal (string clientId, string entityId, string entityType, string msg);
    void resetSignal (string clientId, string entityId, string entityType, string msg);
  };
};

#endif // FRONTENDTOPICS_ICE_
