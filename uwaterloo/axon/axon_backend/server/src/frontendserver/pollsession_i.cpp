/*
 * pollsession_i.cpp
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <sstream>

#include "pollsession_i.h"

class PollCallbackAdapter : public ClientMonitorCallbackAdapter
{
public:

  virtual void newBasicPrflMsg (const FrontEndComm::NewBasicPrflMsgEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newSvcRecordMsg (const FrontEndComm::NewSvcRecordMsgEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newEntityMsg (const FrontEndComm::NewEntityMsgEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  void newEntityMsgDataSeq (const FrontEndComm::NewEntityMsgDataSeqEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  void newEntityMsgDataRec (const FrontEndComm::NewEntityMsgDataRecEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newUpdateMsg (const FrontEndComm::NewUpdateMsgEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void newMsgDataSeq (const FrontEndComm::NewMsgDataSeqEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void pong (const FrontEndComm::PongEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  virtual void disconnect (const FrontEndComm::DisconnectEventPtr& ev)
  {
    IceUtil::Mutex::Lock sync (mutex);

    updates.push_back (ev);
  }

  FrontEndComm::ClientMonitorEventSeq getUpdates ()
  {
    IceUtil::Mutex::Lock sync (mutex);

    FrontEndComm::ClientMonitorEventSeq newUpdates;

    newUpdates.swap (updates);

    return newUpdates;
  }

protected:
  FrontEndComm::ClientMonitorEventSeq updates;
  IceUtil::Mutex mutex;
};
typedef IceUtil::Handle <PollCallbackAdapter> PollCallbackAdapterPtr;

/******************************************************/
/******************************************************/

PollSession_I::PollSession_I (const ClientMonitorPtr& cm, const std::string& id,
    const Ice::LoggerPtr& logger, const bool flag) :
  clientMonitor_Ptr (cm),
  clientId (id),
  iceLogger_Ptr (logger),
  logFlag (flag),
  pollCallBackAdapter_Ptr (new PollCallbackAdapter),
  currTimeStamp (IceUtil::Time::now (IceUtil::Time::Monotonic)),
  terminated (false)
{
  clientMonitor_Ptr->registerClient (clientId, pollCallBackAdapter_Ptr);
}

PollSession_I::~PollSession_I()
{}

FrontEndComm::ClientMonitorEventSeq
PollSession_I::getUpdates (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();
  currTimeStamp = IceUtil::Time::now (IceUtil::Time::Monotonic);

  return pollCallBackAdapter_Ptr->getUpdates ();
}

Ice::Long
PollSession_I::sendToEntity (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->sendToEntity (clientId, entityId, entityType, msgType, msg);
}

Ice::Long
PollSession_I::sendToEntityDataSeq (const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->sendToEntityDataSeq (clientId, entityId, entityType, msgType, dataSeq);
}

Ice::Long
PollSession_I::sendToEntityDataRec (const std::string& entityId,
    const std::string& entityType, const std::string& msgType,
    const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->sendToEntityDataRec (clientId, entityId, entityType, msgType, dataRec);
}

Ice::Long
PollSession_I::sendRequest (const std::string& destId, const std::string& destType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->handleRequest (clientId, destId, destType, msgType, msg);
}

Ice::Long
PollSession_I::sendStopSignal (const std::string& entityId, const std::string& entityType,
    const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->relayStopSignal (clientId, entityId, entityType, msg);
}

Ice::Long
PollSession_I::sendClearSignal (const std::string& entityId, const std::string& entityType,
    const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->relayClearSignal (clientId, entityId, entityType, msg);
}

Ice::Long
PollSession_I::sendResetSignal (const std::string& entityId, const std::string& entityType,
    const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->relayResetSignal (clientId, entityId, entityType, msg);
}

Ice::Long
PollSession_I::ping (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->ping (clientId);
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

          oss << "PollSession: " << "'" << clientId << "' ";
          oss << "tried to terminate an already terminated session.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  try
    {
      curr.adapter->remove (curr.id);
      clientMonitor_Ptr->deregisterClient (clientId);
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

      oss << "PollSession: " << " session of '" << clientId << "' is terminated.";
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

          oss << "PollSession: " << "'" << clientId << "' ";
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

          oss << "PollSession: " << "'" << clientId << "' ";
          oss << "requested session updates list through a terminated session.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }
      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }
}
