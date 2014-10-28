/*
 * clientmonitor.cpp
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <sstream>

#include "clientmonitor.h"
#include "../util/xmlglobalserviceparser.h"
#include "../common/global.h"
#include "../common/status.h"

ClientMonitor::ClientMonitor (const FrontEndComm::FrontEndToEntityCommPrx& prx,
    const Ice::LoggerPtr& logger, const bool& flag) :
  frontEndToEntityComm_Prx (prx),
  iceLogger_Ptr (logger),
  logFlag (flag)
{
  loadGlobalServices ();
}

ClientMonitor::~ClientMonitor ()
{}

int
ClientMonitor::checkRegRequest (const std::string& msg, std::string *clientId)
{
  std::string prefix;
  std::string body;

  IceUtil::Mutex::Lock sync (dataMutex);

  if (!extractInitMsg (msg, &prefix, clientId, &body) || prefix != CLIENT || body != INIT)
      return BADMSG;

  if (exists (*clientId))
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "ClientMonitor: client '" << *clientId << "' already registered";
          iceLogger_Ptr->trace ("Warning", oss.str ());
        }

      return ALRDYEXIST;
    }

  return OK;
}

void
ClientMonitor::registerClient (const std::string& clientId,
    const ClientMonitorCallbackAdapterPtr& callback)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;
  std::map <const std::string, std::string>::iterator it;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, FRONTEND_SERVER, FRONTEND_SERVER, INFO,
      REGISTERED);

  clientMonitorCallbackMap[clientId] = callback;
  clientMonitorCallbackMap[clientId]->newUpdateMsg (ev);
  /// Send all entity profiles to each newly registered client
  for (it = entityBasicPrflMap.begin (); it != entityBasicPrflMap.end (); ++it)
    {
      FrontEndComm::NewBasicPrflMsgEventPtr ev;

      ev = new FrontEndComm::NewBasicPrflMsgEvent (timestamp, (*it).first, ENTITY, (*it).second);
      clientMonitorCallbackMap[clientId]->newBasicPrflMsg (ev);
    }

  generalInfo ();
}

bool
ClientMonitor::deregisterClient (const std::string& clientId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  std::map <const std::string, std::string>::iterator it;

  if (!exists (clientId))
      return false;

  clientMonitorCallbackMap.erase (clientId);
  /// If client had checked-out entities notify the backend server
  for (it = entityToClientMap.begin (); it != entityToClientMap.end (); ++it)
    {
      /// If an entity was previously checked-out by the recently de-registered client
      if ((*it).second == clientId)
        {
          frontEndToEntityComm_Prx->clientUpdate (clientId, (*it).first, ENTITY, ALERT,
              CLIENT_UNREGISTERED);
          entityToClientMap.erase ((*it).first);
        }
    }

  generalInfo ();

  return true;
}

Ice::Long
ClientMonitor::sendToEntity (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, std::string msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  // Relay client message to entity server
  frontEndToEntityComm_Prx->clientMsg (clientId, entityId, entityType, msgType, msg);

  return timestamp;
}

Ice::Long
ClientMonitor::sendToEntityDataSeq (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  // Relay client message to entity server
  frontEndToEntityComm_Prx->clientMsgDataSeq (clientId, entityId, entityType, msgType, dataSeq);

  return timestamp;
}

Ice::Long
ClientMonitor::sendToEntityDataRec (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataRecord& dataRec)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  // Relay client message to entity server
  frontEndToEntityComm_Prx->clientMsgDataRec (clientId, entityId, entityType, msgType, dataRec);

  return timestamp;
}

Ice::Long
ClientMonitor::handleRequest (const std::string& clientId, const std::string& destId,
    const std::string& destType, const std::string& msgType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// Message will contain an entity ID
  if (exists (clientId))
    {
      /// If the messages is destined for an entity
      if (destType == ENTITY)
        {
          /// If it is a request to check-out an entity
          if (msg == CHECKOUT_ENTITY)
            {
              timestamp = IceUtil::Time::now ().toMilliSeconds ();
              /// If entity was previously checked-out inform the requesting client
              if (entityToClientMap[destId].size () != 0)
                {
                  ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, destId,
                      ENTITY, INFO, ENTITY_DENIED);
                }
              /// Inform client that its request was granted
              else
                {
                  ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, destId,
                      ENTITY, INFO, ENTITY_GRANTED);

                  frontEndToEntityComm_Prx->clientRequest (clientId, destId, destType);
                  entityToClientMap[destId] = clientId;
                }
            }
          /// If it is a request to release an entity
          else if (msg == RELEASE_ENTITY)
            {
              /// If entity was already released send a warning
              if (entityToClientMap[destId].size () == 0)
                {
                  timestamp = IceUtil::Time::now ().toMilliSeconds ();
                  ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, destId,
                      ENTITY, WARN, ENTITY_ALREADY_RELEASED);

                  entityToClientMap.erase (destId);
                }
              /// Send a confirmation message
              else
                {
                  ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, destId,
                      ENTITY, INFO, ENTITY_RELEASED);

                  entityToClientMap.erase (destId);
                  frontEndToEntityComm_Prx->clientUpdate (clientId, destId, destType, ALERT, msg);
                }
            }

          clientMonitorCallbackMap[clientId]->newUpdateMsg (ev);
        }
      /// If the messages is destined for the server
      else if (destType == FRONTEND_SERVER)
        {
          /// If it is a data request
          if (msgType == DATAREQ)
            {
              /// If requesting any loaded global services
              if (msg == GLOBALSVC)
                {
                  /// If no data were loaded
                  if (globalServicesData.size () > 0)
                    {
                      FrontEndComm::NewMsgDataSeqEventPtr ev;

                      ev = new FrontEndComm::NewMsgDataSeqEvent (timestamp, "",
                          FRONTEND_SERVER, DATA, globalServicesData);

                      clientMonitorCallbackMap[clientId]->newMsgDataSeq (ev);
                    }
                  else
                    {
                      FrontEndComm::NewUpdateMsgEventPtr ev;

                      ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, "",
                          FRONTEND_SERVER, INFO, NODATA);

                      clientMonitorCallbackMap[clientId]->newUpdateMsg (ev);
                    }
                }
            }
        }
      /// If this is not the intended destination
      else
        {
          if (logFlag)
            {
              std::ostringstream oss;

              oss << "ClientMonitor: unable handle requests from '" << clientId << "'";
              iceLogger_Ptr->trace ("Warning", oss.str ());
            }
        }
    }

  return timestamp;
}

Ice::Long
ClientMonitor::relayStopSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// If client exists
  if (exists (clientId))
    {
      /// If entity is not checked-out by client or previously checked-out by other client
      if (entityToClientMap[entityId].size () == 0 || entityToClientMap[entityId] != clientId)
        {
          ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, entityId, ENTITY, INFO,
              ENTITY_CONTROL_DENIED);
          clientMonitorCallbackMap[clientId]-> newUpdateMsg (ev);
        }
      /// Relay the stop signal to the entity server
      else
        {
          frontEndToEntityComm_Prx->stopSignal (clientId, entityId, entityType, msg);
        }
    }

  return timestamp;
}

Ice::Long
ClientMonitor::relayClearSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// If client exists
  if (exists (clientId))
    {
      /// If entity is not checked-out by client or previously checked-out by other client
      if (entityToClientMap[entityId].size () == 0 || entityToClientMap[entityId] != clientId)
        {
          ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, entityId, ENTITY, INFO,
              ENTITY_CONTROL_DENIED);
          clientMonitorCallbackMap[clientId]-> newUpdateMsg (ev);
        }
      /// Relay the clear signal to the entity server
      else
        {
          frontEndToEntityComm_Prx->clearSignal (clientId, entityId, entityType, msg);
        }
    }

  return timestamp;
}

Ice::Long
ClientMonitor::relayResetSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// If client exists
  if (exists (clientId))
    {
      /// If entity is not checked-out by client or previously checked-out by other client
      if (entityToClientMap[entityId].size () == 0 || entityToClientMap[entityId] != clientId)
        {
          ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, entityId, ENTITY, INFO,
              ENTITY_CONTROL_DENIED);
          clientMonitorCallbackMap[clientId]-> newUpdateMsg (ev);
        }
      /// Relay the reset signal to the entity server
      else
        {
          frontEndToEntityComm_Prx->resetSignal (clientId, entityId, entityType, msg);
        }
    }

  return timestamp;
}

Ice::Long
ClientMonitor::ping (const std::string& clientId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;
  FrontEndComm::PongEventPtr ev;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new FrontEndComm::PongEvent (timestamp);

  clientMonitorCallbackMap[clientId]-> pong (ev);

  return timestamp;
}

void
ClientMonitor::entityBasicPrflUpdate (const std::string& entityId, const std::string& entityType,
    const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewBasicPrflMsgEventPtr ev;
  IceUtil::Int64 timestamp;
  std::map <const std::string, ClientMonitorCallbackAdapterPtr>::iterator it;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// Update basic profile
  entityBasicPrflMap[entityId] = msg;
  /// Create an update event and relay it to all online clients
  ev = new FrontEndComm::NewBasicPrflMsgEvent (timestamp, entityId, entityType, msg);
  /// Relay service record to all connected clients
  for (it = clientMonitorCallbackMap.begin (); it != clientMonitorCallbackMap.end (); ++it)
      (*it).second->newBasicPrflMsg (ev);
}

void
ClientMonitor::entitySvcRecord (const std::string& entityId, const std::string& entityType,
    int recordId, int count, const CommUtil::DataRecord& svcRecord)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewSvcRecordMsgEventPtr ev;
  IceUtil::Int64 timestamp;
  std::string clientId;

  clientId = entityToClientMap[entityId];
  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (clientId.size () == 0 || !exists (clientId))
    {
      entityToClientMap.erase (entityId);
      frontEndToEntityComm_Prx->clientUpdate ("", entityId, entityType, WARN, CLIENT_NOT_EXIST);
    }
  else
    {
      ev = new FrontEndComm::NewSvcRecordMsgEvent (timestamp, entityId, entityType, recordId,
          count, svcRecord);

      clientMonitorCallbackMap[clientId]->newSvcRecordMsg (ev);
    }
}

void
ClientMonitor::entityMsg (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewEntityMsgEventPtr ev;
  IceUtil::Int64 timestamp;
  std::string clientId;

  clientId = entityToClientMap[entityId];
  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (clientId.size () == 0 || !exists (clientId))
    {
      entityToClientMap.erase (entityId);
      frontEndToEntityComm_Prx->clientUpdate ("", entityId, entityType, WARN, CLIENT_NOT_EXIST);
    }
  else
    {
      ev = new FrontEndComm::NewEntityMsgEvent (timestamp, entityId, entityType, msgType, msg);

      clientMonitorCallbackMap[clientId]->newEntityMsg (ev);
    }
}

void
ClientMonitor::entityMsgDataSeq (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const CommUtil::DataSeq& dataSeq)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewEntityMsgDataSeqEventPtr ev;
  IceUtil::Int64 timestamp;
  std::string clientId;

  clientId = entityToClientMap[entityId];
  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (clientId.size () == 0 || !exists (clientId))
    {
      entityToClientMap.erase (entityId);
      frontEndToEntityComm_Prx->clientUpdate ("", entityId, entityType, WARN, CLIENT_NOT_EXIST);
    }
  else
    {
      ev = new FrontEndComm::NewEntityMsgDataSeqEvent (timestamp, entityId, entityType, msgType,
          dataSeq);

      clientMonitorCallbackMap[clientId]->newEntityMsgDataSeq (ev);
    }
}

void
ClientMonitor::entityMsgDataRec (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const CommUtil::DataRecord& dataRec)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewEntityMsgDataRecEventPtr ev;
  IceUtil::Int64 timestamp;
  std::string clientId;

  clientId = entityToClientMap[entityId];
  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (clientId.size () == 0 || !exists (clientId))
    {
      entityToClientMap.erase (entityId);
      frontEndToEntityComm_Prx->clientUpdate ("", entityId, entityType, WARN, CLIENT_NOT_EXIST);
    }
  else
    {
      ev = new FrontEndComm::NewEntityMsgDataRecEvent (timestamp, entityId, entityType, msgType,
          dataRec);

      clientMonitorCallbackMap[clientId]->newEntityMsgDataRec (ev);
    }
}

void
ClientMonitor::entityUpdate (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  FrontEndComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;
  std::map <const std::string, ClientMonitorCallbackAdapterPtr>::iterator it;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new FrontEndComm::NewUpdateMsgEvent (timestamp, entityId, entityType, msgType, msg);
  /// If it is an ALERT message
  if (msgType == ALERT)
    {
      /// If an entity went offline delete all its records
      if (msg == ENTITY_UNREGISTERED)
        {
          entityBasicPrflMap.erase (entityId);
          entityToClientMap.erase (entityId);
        }
      /// An entity update is relayed to all clients
      for (it = clientMonitorCallbackMap.begin (); it != clientMonitorCallbackMap.end (); ++it)
          (*it).second->newUpdateMsg (ev);
    }
}

bool
ClientMonitor::exists (const std::string& clientId)
{
  if (clientMonitorCallbackMap.find (clientId) == clientMonitorCallbackMap.end ())
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "ClientMonitor: client '" << clientId << "' is not already registered.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }

      return false;
    }

  return true;
}

bool
ClientMonitor::extractInitMsg (const std::string& msg, std::string *prefix, std::string *id,
    std::string *body)
{
  char str[msg.size ()];
  char *token;

  strncpy (str, msg.c_str (), msg.size ());
  str[msg.size ()] = '\0';
  token = strtok (str, DELIM);

  if (token == NULL)
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Error", "ClientMonitor: Message is not properly formatted.");

      return false;
    }
  else
    {
      *prefix = token;
    }

  token = strtok (NULL, DELIM);

  if (token == NULL)
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Error", "ClientMonitor: ID is missing.");

      return false;
    }
  else
    {
      *id = token;
    }

  token = strtok (NULL, DELIM);

  if (token == NULL)
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Error", "ClientMonitor: Message body is missing.");

      return false;
    }
  else
    {
      *body = token;
    }

  return true;
}

void
ClientMonitor::loadGlobalServices ()
{
  XmlGlobalServiceParser parser;
  std::vector <std::string> data;
  std::ostringstream oss;

  parser.init (&data);

  if (!parser.setFile (GLOBALSVC_FILE))
    {
      if (logFlag)
        {
          oss << "ClientMonitor: failed to open global services file: ";
          oss << parser.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }
    }
  else if (!parser.parse ())
    {
      if (logFlag)
        {
          oss << "ClientMonitor: failed to load global services: ";
          oss << parser.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }
    }
  else
    {
      for (unsigned int i = 0; i < data.size (); ++i)
          globalServicesData.push_back (data[i]);

      if (logFlag)
        {
          oss << "ClientMonitor: loaded global services.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }
    }
}

void
ClientMonitor::generalInfo ()
{

  std::ostringstream oss;

  oss << "ClientMonitor: " << clientMonitorCallbackMap.size () << " client(s) registered.";
  iceLogger_Ptr->trace ("Info", oss.str ());
}
