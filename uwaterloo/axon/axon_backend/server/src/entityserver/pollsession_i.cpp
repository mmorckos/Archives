/*
 * pollsession_i.cpp
 *
 *  Created on: 2011-12-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <sstream>

#include "pollsession_i.h"

class PollCallbackAdapter : public EntityMonitorCallbackAdapter
{
public:

  virtual void newClientMsg (const EntityComm::NewClientMsgEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newClientMsgDataSeq (const EntityComm::NewClientMsgDataSeqEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newClientMsgDataRec (const EntityComm::NewClientMsgDataRecEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newUpdateMsg (const EntityComm::NewUpdateMsgEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newEntityMsg (const EntityComm::NewEntityMsgEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newEntityMsgDataSeq (const EntityComm::NewEntityMsgDataSeqEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newEntityMsgDataRec (const EntityComm::NewEntityMsgDataRecEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void stopSignal (const EntityComm::StopEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void clearSignal (const EntityComm::ClearEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void resetSignal (const EntityComm::ResetEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void pong (const EntityComm::PongEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void disconnect (const EntityComm::DisconnectEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  EntityComm::EntityMonitorEventSeq getUpdates ()
  {
    IceUtil::Mutex::Lock sync (mutex);

    EntityComm::EntityMonitorEventSeq newUpdates;

    newUpdates.swap (updates);

    return newUpdates;
  }

protected:
  EntityComm::EntityMonitorEventSeq updates;
  IceUtil::Mutex mutex;
};
typedef IceUtil::Handle <PollCallbackAdapter> PollCallbackAdapterPtr;

/******************************************************/
/******************************************************/

PollSession_I::PollSession_I (const EntityMonitorPtr& em, const std::string& id,
    const Ice::LoggerPtr& logger, const bool flag) :
  entityMonitor_Ptr (em),
  entityId (id),
  iceLogger_Ptr (logger),
  logFlag (flag),
  pollCallBackAdapter_Ptr (new PollCallbackAdapter),
  currTimeStamp (IceUtil::Time::now (IceUtil::Time::Monotonic)),
  terminated (false)
{
  entityMonitor_Ptr->registerEntity (entityId, pollCallBackAdapter_Ptr);
}

PollSession_I::~PollSession_I()
{}

EntityComm::EntityMonitorEventSeq
PollSession_I::getUpdates (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  currTimeStamp = IceUtil::Time::now (IceUtil::Time::Monotonic);

  common ();

  return pollCallBackAdapter_Ptr->getUpdates ();
}

Ice::Long
PollSession_I::sendBasicPrfl (const std::string& entityType, const std::string& msg,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->updateBasicPrfl (entityId, entityType, msg);
}

Ice::Long
PollSession_I::sendSvcPrflRecord (const std::string& entityType, int recordId, int count,
    const CommUtil::DataRecord& svcRecord, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->relaySvcRecord (entityId, entityType, recordId, count, svcRecord);
}

Ice::Long
PollSession_I::sendToClient (const std::string& clientId, const std::string& entityType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToClient (entityId, clientId, entityType, msgType, msg);
}

Ice::Long
PollSession_I::sendToClientDataSeq (const std::string& clientId, const std::string& entityType,
    const std::string& msgType, const CommUtil::DataSeq& dataSeq, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToClientDataSeq (entityId, clientId, entityType, msgType, dataSeq);
}


Ice::Long
PollSession_I::sendToClientDataRec (const std::string& clientId, const std::string& entityType,
    const std::string& msgType, const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToClientDataRec (entityId, clientId, entityType, msgType, dataRec);
}

Ice::Long
PollSession_I::sendToEntity (const std::string& srcEntityId, const std::string& destEntityId,
    const std::string& clientId, const std::string& srcEntityType,
    const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
    const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToEntity (srcEntityId, destEntityId, clientId, srcEntityType,
      destEntityType, clientType, msgType, msg);
}

Ice::Long
PollSession_I::sendToEntityDataSeq (const std::string& srcEntityId, const std::string& destEntityId,
    const std::string& clientId, const std::string& srcEntityType,
    const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
    const CommUtil::DataSeq& dataSeq, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToEntityDataSeq (srcEntityId, destEntityId, clientId, srcEntityType,
      destEntityType, clientType, msgType, dataSeq);
}

Ice::Long
PollSession_I::sendToEntityDataRec (const std::string& srcEntityId, const std::string& destEntityId,
    const std::string& clientId, const std::string& srcEntityType,
    const std::string& destEntityType, const std::string& clientType, const std::string& msgType,
    const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToEntityDataRec (srcEntityId, destEntityId, clientId, srcEntityType,
      destEntityType, clientType, msgType, dataRec);
}

Ice::Long
PollSession_I::sendUpdate (const std::string& destId, const std::string& destType,
    const std::string& entityType, const std::string& msgType, const std::string& msg,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->handleUpdate (entityId, destId, destType, entityType, msgType, msg);
}

Ice::Long
PollSession_I::sendUpdateDataSeq (const std::string& destId, const std::string& destType,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->handleUpdateDataSeq (entityId, destId, destType, entityType, msgType,
      dataSeq);
}

Ice::Long
PollSession_I::ping (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->ping (entityId);
}

void
PollSession_I::destroy (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  if (terminated)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "PollSession: " << "'" << entityId << "' ";
          oss << "tried to terminate an already terminated session.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  try
    {
      curr.adapter->remove (curr.id);
      entityMonitor_Ptr->deregisterEntity (entityId);
    }
  catch (const Ice::ObjectAdapterDeactivatedException& ex)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "PollSession: " << "server is shutting down.";
          iceLogger_Ptr->trace ("Warning", oss.str ());
        }
    }

  if (logFlag)
    {
      std::ostringstream oss;

      oss << "PollSession: " << "'" << entityId << "' is terminated.";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }

  terminated = true;
}

IceUtil::Time
PollSession_I::timestamp() const
{
  IceUtil::Mutex::Lock sync (mutex);

  if (terminated)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "PollSession: " << "'" << entityId << "' ";
          oss << "requested timestamp of a terminated session.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  return currTimeStamp;
}

void
PollSession_I::common ()
{
  if (terminated)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "PollSession: " << "'" << entityId << "' ";
          oss << "requested session updates list through a terminated session.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }
      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }
}
