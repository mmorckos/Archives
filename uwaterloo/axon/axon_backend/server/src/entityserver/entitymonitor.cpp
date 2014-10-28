/*
 * entitymonitor.cpp
 *
 *  Created on: 2011-12-06
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <sstream>

#include "entitymonitor.h"
#include "../common/global.h"
#include "../common/status.h"

/*! \brief Constructor.
 *
 * \param logger of type Ice::LoggerPtr, the ICE logging facility.
 * \param flag of type bool, to toggle logging.
 */

EntityMonitor::EntityMonitor (const EntityComm::EntityToFrontEndCommPrx& prx,
    const Ice::LoggerPtr& logger, const bool& flag) :
  entityToFrontEndComm_Prx (prx),
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

/*! \brief Constructor
 */
EntityMonitor::~EntityMonitor ()
{}

/*! \brief Entity registration request checker function.
 *
 * This function checks that a request message from an entity is in the proper format.
 * \param msg of type string, the request message.
 * \param entityId of type string, the uuid of the entity.
 * \return status of type int, the result of the checking process.
 */
int
EntityMonitor::checkRegRequest (const std::string& msg, std::string *entityId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  std::string prefix;
  std::string body;

  if (!extractInitMsg (msg, &prefix, entityId, &body) || prefix != ENTITY || body != INIT)
      return BADMSG;

  if (exists (*entityId))
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "EntityMonitor: entity '" << *entityId << "' already registered";
          iceLogger_Ptr->trace ("Warning", oss.str ());
        }

      return ALRDYEXIST;
    }

  return OK;
}

/*! \brief Entity registration function.
 *
 * This function registers an entity by storing its uuid and callback adapter.
 * \param entityId of type string, the uuid of the entity.
 * \param callback of type EntityMonitorCallbackAdapterPtr, the entities callback adapter.
 * \sa unregisterEntity (const std::string entityId)
 */
void
EntityMonitor::registerEntity (const std::string& entityId,
    const EntityMonitorCallbackAdapterPtr& callback)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::NewUpdateMsgEvent (timestamp, ENTITY_SERVER, ENTITY_SERVER, ENTITY, INFO,
      REGISTERED);

  entityMonitorCallbackMap[entityId] = callback;
  statusMap[entityId] = IDLE;
  entityMonitorCallbackMap[entityId]->newUpdateMsg (ev);

  generalInfo ();
}

/*! \brief Entity deregistration function.
 *
 * This function deregister an entity in case of communication termination or loss.
 * \param entityId of type string, the uuid of the entity.
 * \param return true if entity deregistration is success. false if entity did not exist.
 */
bool
EntityMonitor::deregisterEntity (const std::string& entityId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;
  EntityComm::NewUpdateMsgEventPtr ev;
  std::string tmp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (!exists (entityId))
      return false;
  /// Deleting all information stored for that entity
  entityMonitorCallbackMap.erase (entityId);
  basicPrflMap.erase (entityId);
  healthPrflMap.erase (entityId);
  statusMap.erase (entityId);
  /// Update the frontend server
  entityToFrontEndComm_Prx->entityUpdate (entityId, ENTITY, ALERT, ENTITY_UNREGISTERED);
  /// Update the association map and inform other collaborating entities (if any)
  for (unsigned int i = 0; i < associationMap[entityId].size (); ++i)
    {
      if (exists (associationMap[entityId][i]))
        {
          ev = new EntityComm::NewUpdateMsgEvent (timestamp, ENTITY_SERVER,
              ENTITY_SERVER, ENTITY, ALERT, ENTITY_DROPPED);
          entityMonitorCallbackMap[associationMap[entityId][i]]->newUpdateMsg (ev);
        }
    }
  associationMap.erase (entityId);

  if (logFlag)
    {
      std::ostringstream oss;

      oss << "EntityMonitor: entity '" << entityId << "' de-registered";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }

  generalInfo ();

  return true;
}

Ice::Long
EntityMonitor::updateBasicPrfl (const std::string& entityId, const std::string& entityType,
    const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  /// If entity exists
  if (exists (entityId))
    {
      basicPrflMap[entityId] = msg;
      ev = new EntityComm::NewUpdateMsgEvent (timestamp, ENTITY_SERVER, ENTITY_SERVER, entityType,
          INFO, RECEIVED);
      /// Update the frontend server
      entityToFrontEndComm_Prx->entityBasicPrflUpdate (entityId, entityType, msg);
      /// Reply success to entity
      entityMonitorCallbackMap[entityId]->newUpdateMsg (ev);
    }

  return timestamp;
}

Ice::Long
EntityMonitor::relaySvcRecord (const std::string& entityId, const std::string& entityType,
    int recordId, int count, const CommUtil::DataRecord& svcRecord)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// Relay the service record received from entity to the frontend server
  entityToFrontEndComm_Prx->entitySvcRecord (entityId, entityType, recordId, count, svcRecord);

  return timestamp;
}

Ice::Long
EntityMonitor::sendToClient (const std::string& entityId, const std::string& clientId,
    const std::string& entityType, const std::string& msgType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// Relay message received from entity to the frontend server
  entityToFrontEndComm_Prx->entityMsg (entityId, entityType, msgType, msg);

  return timestamp;
}

Ice::Long
EntityMonitor::sendToClientDataSeq (const std::string& entityId, const std::string& clientId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// Relay message received from entity to the front-end server
  entityToFrontEndComm_Prx->entityMsgDataSeq (entityId, entityType, msgType, dataSeq);

  return timestamp;
}

Ice::Long
EntityMonitor::sendToClientDataRec (const std::string& entityId, const std::string& clientId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataRecord& dataRec)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  /// Relay message received from entity to the front-end server
  entityToFrontEndComm_Prx->entityMsgDataRec (entityId, entityType, msgType, dataRec);

  return timestamp;
}

Ice::Long
EntityMonitor::handleUpdate (const std::string& entityId, const std::string& destId,
    const std::string& destType, const std::string& entityType, const std::string& msgType,
    const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;
  std::ostringstream oss;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (exists (entityId))
    {
      /// If message is meant to be received here
      if (destType == ENTITY_SERVER)
        {
          /// If it is a status update, update the entities status and notify frontend server
          if (msgType == STAT)
            {
              ev = new EntityComm::NewUpdateMsgEvent (timestamp, ENTITY_SERVER, ENTITY_SERVER,
                  entityType, INFO, RECEIVED);

              statusMap[entityId] = msg;
              /// Update frontend server
              entityToFrontEndComm_Prx->entityUpdate (entityId, entityType, STAT, msg);
              /// Reply outcome to entity
              entityMonitorCallbackMap[entityId]->newUpdateMsg (ev);
            }
          /// If it is a health update
          else if (msgType == HLTH)
            {
              /// TODO
            }
          /// If it is a request
          else if (msgType == REQUEST)
            {
              /// Request to terminate collaboration
              if (msg == TERMINATE_COLLABORATION)
                {
                  for (unsigned int i = 0; i < associationMap[entityId].size (); ++i)
                    {
                      if (exists (associationMap[entityId][i]))
                        {
                          ev = new EntityComm::NewUpdateMsgEvent (timestamp, ENTITY_SERVER,
                              ENTITY_SERVER, ENTITY, ALERT, ENTITY_RELEASED);
                          entityMonitorCallbackMap[associationMap[entityId][i]]->newUpdateMsg (ev);
                        }
                    }
                  associationMap.erase (entityId);
                }
            }
          /// If it is an unknown update type
          else
            {
              ev = new EntityComm::NewUpdateMsgEvent (timestamp, ENTITY_SERVER, ENTITY_SERVER,
                  entityType, ERROR, BAD_MSG);
              /// Reply outcome to entity
              entityMonitorCallbackMap[entityId]->newUpdateMsg (ev);

              if (logFlag)
                {
                  oss.str ("");
                  oss << "EntityMonitor: unknown update from: '" << entityId << "'";
                  iceLogger_Ptr->trace ("Error", oss.str ());
                }
            }
        }
      else
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityMonitor: unable handle requests from: '" << entityId << "'";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }
        }
    }
  else
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityMonitor: unknown entity: '" << entityId << "'";
          iceLogger_Ptr->trace ("Warning", oss.str ());
        }
    }

  std::cout << "DONE WITH THE UPPPPPPPPDATE" << std::endl;
  return timestamp;
}

Ice::Long
EntityMonitor::handleUpdateDataSeq (const std::string& entityId, const std::string& destId,
    const std::string& destType, const std::string& entityType, const std::string& msgType,
    const CommUtil::DataSeq& dataSeq)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;
  unsigned int i;
  std::ostringstream oss;
  EntityComm::NewUpdateMsgEventPtr ev0;
  EntityComm::NewEntityMsgEventPtr ev1;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (exists (entityId))
    {
      /// If it is a message indicating a potential collaboration scheme
      if (msgType == COLLABORATION)
        {
          /// Check that all requested entities are registered
          for (i = 1; i < dataSeq.size (); ++i)
            {
              if (!exists (dataSeq[i]))
                {
                  ev0 = new EntityComm::NewUpdateMsgEvent (timestamp, ENTITY_SERVER, ENTITY_SERVER,
                      entityType, ALERT, ENTITY_NOEXIST);
                  entityMonitorCallbackMap[entityId]->newUpdateMsg (ev0);
                  break;
                }
            }
          /// Add all entities to the associative lists of each others
          if (i == dataSeq.size ())
            {
              for (i = 1; i < dataSeq.size (); ++i)
                {
                  ev1 = new EntityComm::NewEntityMsgEvent (timestamp, entityId, dataSeq[i],
                      dataSeq[0], ENTITY, ENTITY, CLIENT, REQUEST, COLLABORATION);
                  associationMap[entityId].push_back (dataSeq[i]);
                  associationMap[dataSeq[i]].push_back (entityId);
                  entityMonitorCallbackMap[dataSeq[i]]->newEntityMsg (ev1);
                }
            }
        }
    }
  else
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityMonitor: unknown entity: '" << entityId << "'";
          iceLogger_Ptr->trace ("Warning", oss.str ());
        }
    }

  return timestamp;
}

Ice::Long
EntityMonitor::sendToEntity (const std::string& srcEntityId, const std::string& destEntityId,
    const std::string& clientId, const std::string& srcEntityType,
    const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
    const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;
  EntityComm::NewEntityMsgEventPtr ev;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (exists (srcEntityId) && exists (destEntityId))
    {
      ev = new EntityComm::NewEntityMsgEvent (timestamp, srcEntityId, destEntityId,
          clientId, srcEntityType, destEntityType, clientType, msgType, msg);
      entityMonitorCallbackMap[destEntityId]->newEntityMsg (ev);
    }

  return timestamp;
}

Ice::Long
EntityMonitor::sendToEntityDataSeq (const std::string& srcEntityId, const std::string& destEntityId,
    const std::string& clientId, const std::string& srcEntityType,
    const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
    const CommUtil::DataSeq& dataSeq)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;
  EntityComm::NewEntityMsgDataSeqEventPtr ev;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  if (exists (srcEntityId) && exists (destEntityId))
    {
      ev = new EntityComm::NewEntityMsgDataSeqEvent (timestamp, srcEntityId, destEntityId,
          clientId, srcEntityType, destEntityType, clientType, msgType, dataSeq);
      entityMonitorCallbackMap[destEntityId]->newEntityMsgDataSeq (ev);
    }

  return timestamp;
}

Ice::Long
EntityMonitor::sendToEntityDataRec (const std::string& srcEntityId, const std::string& destEntityId,
    const std::string& clientId, const std::string& srcEntityType,
    const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
    const CommUtil::DataRecord& dataRec)
{
  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  /// TODO Relay the message to the destination entity

  return timestamp;
}

Ice::Long
EntityMonitor::ping (const std::string& entityId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  IceUtil::Int64 timestamp;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();

  return timestamp;
}

/*! \brief This functions checks if an entity is registered or not.
 *
 * \param entityId of type string, the uuid of the entity.
 * \param return true if the entity exists. false otherwise.
 */
bool
EntityMonitor::exists (const std::string& entityId)
{
  if (entityMonitorCallbackMap.find (entityId) == entityMonitorCallbackMap.end ())
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "EntityMonitor: entity '" << entityId << "' is not already registered.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }

      return false;
    }

  return true;
}

bool
EntityMonitor::clientMsg (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewClientMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::NewClientMsgEvent (timestamp, clientId, entityType, msgType, msg);

  entityMonitorCallbackMap[entityId]->newClientMsg (ev);

  return true;
}

bool
EntityMonitor::clientMsgDataSeq (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewClientMsgDataSeqEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::NewClientMsgDataSeqEvent (timestamp, clientId, entityType, msgType,
      dataSeq);

  entityMonitorCallbackMap[entityId]->newClientMsgDataSeq (ev);

  return true;
}

bool
EntityMonitor::clientMsgDataRec (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataRecord& dataRec)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewClientMsgDataRecEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::NewClientMsgDataRecEvent (timestamp, clientId, entityType, msgType,
      dataRec);

  entityMonitorCallbackMap[entityId]->newClientMsgDataRec (ev);

  return true;
}


bool
EntityMonitor::clientRequest (const std::string& clientId, const std::string& entityId,
    const std::string& entityType)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::NewUpdateMsgEvent (timestamp, clientId, CLIENT, entityType, REQUEST,
      CHECKOUT_ENTITY);

  entityMonitorCallbackMap[entityId]->newUpdateMsg (ev);

  return true;
}

bool
EntityMonitor::clientUpdate (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::NewUpdateMsgEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::NewUpdateMsgEvent (timestamp, clientId, CLIENT, entityType, msgType, msg);

  entityMonitorCallbackMap[entityId]->newUpdateMsg (ev);

  return true;
}

bool
EntityMonitor::relayStopSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::StopEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::StopEvent (timestamp, clientId, entityType, msg);

  entityMonitorCallbackMap[entityId]->stopSignal (ev);

  return true;
}

bool
EntityMonitor::relayClearSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::ClearEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::ClearEvent (timestamp, clientId, entityType, msg);

  entityMonitorCallbackMap[entityId]->clearSignal (ev);

  return true;
}

bool
EntityMonitor::relayResetSignal (const std::string& clientId, const std::string& entityId,
    const std::string& entityType, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  EntityComm::ResetEventPtr ev;
  IceUtil::Int64 timestamp;

  if (!exists (entityId))
      return false;

  timestamp = IceUtil::Time::now ().toMilliSeconds ();
  ev = new EntityComm::ResetEvent (timestamp, clientId, entityType, msg);

  entityMonitorCallbackMap[entityId]->resetSignal (ev);

  return true;
}

/*! \brief This function splits a message into a prefix and a body.
 *
 * \param prefix of type string*, which will store the message's prefix.
 * \param msg of type string*, which will store the message's body.
 * \return true if the message is properly formatted. false otherwise.
 */
bool
EntityMonitor::extractMsg (const std::string& msg, std::string *prefix, std::string *body)
{
  size_t pos;

  pos = msg.find (DELIM);

  if (pos == std::string::npos)
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Error", "EntityMonitor: Message is not properly formatted.");

      return false;
    }

  (*prefix) = msg.substr (0, pos - 1);
  (*body) = msg.substr (pos + 1);

  return true;
}

bool
EntityMonitor::extractInitMsg (const std::string& msg, std::string *prefix, std::string *id,
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
          iceLogger_Ptr->trace ("Error", "EntityMonitor: Message is not properly formatted.");

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
          iceLogger_Ptr->trace ("Error", "EntityMonitor: ID is missing.");

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
          iceLogger_Ptr->trace ("Error", "EntityMonitor: Message body is missing.");

      return false;
    }
  else
    {
      *body = token;
    }

  return true;
}

void
EntityMonitor::generalInfo ()
{
  std::ostringstream oss;

  oss << "EntityMonitor: " << entityMonitorCallbackMap.size () << " entity(ies) registered.";
  iceLogger_Ptr->trace ("Info", oss.str ());
}
