/*
 * entitysession_i.cpp
 *
 *  Created on: 2011-12-08
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "session_i.h"

class SessionCallBackAdapter : public EntityMonitorCallbackAdapter
{

public:
  SessionCallBackAdapter (
      const EntityComm::EntityMonitorCallbackPrx& cb, const EntityComm::SessionPrx& session,
      const std::string& id, const Ice::LoggerPtr& logger, const bool flag) :
    entityMonitorCb_prx (cb),
    session_prx (session),
    entityId (id),
    iceLogger_Ptr (logger),
    logFlag (flag)
  {}

  void newClientMsg (const EntityComm::NewClientMsgEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_newClientMsgPtr newClientMsgCb =
        EntityComm::newCallback_EntityMonitorCallback_newClientMsg (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_newClientMsg (ev->timestamp, ev->clientId, ev->entityType,
            ev->msgType, ev->msg, newClientMsgCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newClientMsgDataSeq (const EntityComm::NewClientMsgDataSeqEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_newClientMsgDataSeqPtr newClientMsgDataSeqCb =
        EntityComm::newCallback_EntityMonitorCallback_newClientMsgDataSeq (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_newClientMsgDataSeq (ev->timestamp, ev->clientId,
            ev->entityType, ev->msgType, ev->dataSeq, newClientMsgDataSeqCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newClientMsgDataRec (const EntityComm::NewClientMsgDataRecEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_newClientMsgDataRecPtr newClientMsgDataRecCb =
        EntityComm::newCallback_EntityMonitorCallback_newClientMsgDataRec (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_newClientMsgDataRec (ev->timestamp, ev->clientId,
            ev->entityType, ev->msgType, ev->dataRec, newClientMsgDataRecCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newUpdateMsg (const EntityComm::NewUpdateMsgEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_newUpdateMsgPtr newUpdateMsgCb =
        EntityComm::newCallback_EntityMonitorCallback_newUpdateMsg (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_newUpdateMsg (ev->timestamp, ev->srcId, ev->srcType,
            ev->destType, ev->msgType, ev->msg, newUpdateMsgCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newEntityMsg (const EntityComm::NewEntityMsgEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_newEntityMsgPtr newEntityMsgCb =
        EntityComm::newCallback_EntityMonitorCallback_newEntityMsg (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_newEntityMsg (ev->timestamp, ev->srcEntityId, ev->destEntityId,
            ev->clientId, ev->srcEntityType, ev->destEntityType, ev->clientType, ev->msgType,
            ev->msg, newEntityMsgCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newEntityMsgDataSeq (const EntityComm::NewEntityMsgDataSeqEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_newEntityMsgDataSeqPtr newEntityMsgDataSeqCb =
        EntityComm::newCallback_EntityMonitorCallback_newEntityMsgDataSeq (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_newEntityMsgDataSeq (ev->timestamp, ev->srcEntityId,
            ev->destEntityId, ev->clientId, ev->srcEntityType, ev->destEntityType, ev->clientType,
            ev->msgType, ev->dataSeq, newEntityMsgDataSeqCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newEntityMsgDataRec (const EntityComm::NewEntityMsgDataRecEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_newEntityMsgDataRecPtr newEntityMsgDataRecCb =
        EntityComm::newCallback_EntityMonitorCallback_newEntityMsgDataRec (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_newEntityMsgDataRec (ev->timestamp, ev->srcEntityId,
            ev->destEntityId, ev->clientId, ev->srcEntityType, ev->destEntityType, ev->clientType,
            ev->msgType, ev->dataRec, newEntityMsgDataRecCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void stopSignal (const EntityComm::StopEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_stopSignalPtr stopSignalCb =
        EntityComm::newCallback_EntityMonitorCallback_stopSignal (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_stopSignal (ev->timestamp, ev->clientId, ev->entityType,
            ev->msg, stopSignalCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void clearSignal (const EntityComm::ClearEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_clearSignalPtr clearSignalCb =
        EntityComm::newCallback_EntityMonitorCallback_clearSignal (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_clearSignal (ev->timestamp, ev->clientId, ev->entityType,
            ev->msg, clearSignalCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void resetSignal (const EntityComm::ResetEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_resetSignalPtr resetSignalCb =
        EntityComm::newCallback_EntityMonitorCallback_resetSignal (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);

    try
      {
        entityMonitorCb_prx->begin_resetSignal (ev->timestamp, ev->clientId, ev->entityType,
            ev->msg, resetSignalCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void pong (const EntityComm::PongEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_pongPtr pongCb =
        EntityComm::newCallback_EntityMonitorCallback_pong (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        entityMonitorCb_prx->begin_pong (ev->timestamp, pongCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void disconnect (const EntityComm::DisconnectEventPtr& ev)
  {
    EntityComm::Callback_EntityMonitorCallback_disconnectPtr disconnectCb =
        EntityComm::newCallback_EntityMonitorCallback_disconnect (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        entityMonitorCb_prx->begin_disconnect (ev->timestamp, disconnectCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void success ()
  {}

  void failure (const Ice::Exception& ex)
  {
    if (logFlag)
      {
        std::ostringstream oss;

        oss << "SessionCallBackAdapter: '" << entityId << "' will be terminated.";
        iceLogger_Ptr->trace ("Error", oss.str ());
      }

    try
      {
        /// FIXME Omitted due to potential deadlock occurrence
//        session_prx->destroy ();
      }
    catch (const Ice::LocalException& ex)
    {}
  }

protected:
  const EntityComm::EntityMonitorCallbackPrx entityMonitorCb_prx;
  const EntityComm::SessionPrx session_prx;
  const std::string entityId;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;
};

/******************************************************/
/******************************************************/

Session_I::Session_I (const EntityMonitorPtr& em, const std::string& id,
    const Ice::LoggerPtr& logger, const bool flag) :
  entityMonitor_Ptr (em),
  entityId (id),
  iceLogger_Ptr (logger),
  logFlag (flag),
  terminated (false)
{}

Session_I::~Session_I ()
{}

void
Session_I::setCallback (const EntityComm::EntityMonitorCallbackPrx& callback,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  Ice::Context ctx;

  if (terminated)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "Session: " << "'" << entityId << "' ";
          oss << "terminated. Cannot set session callback.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  if (entityMonitor_Cb || !callback)
      return;

  ctx["_fwd"] = "o";
  entityMonitor_Cb = new SessionCallBackAdapter (callback->ice_context (ctx),
      EntityComm::SessionPrx::uncheckedCast (curr.adapter->createProxy (curr.id)), entityId,
      iceLogger_Ptr, logFlag);

  entityMonitor_Ptr->registerEntity (entityId, this->entityMonitor_Cb);
}

Ice::Long
Session_I::sendBasicPrfl (const std::string& entityType, const std::string& msg,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->updateBasicPrfl (entityId, entityType, msg);
}

Ice::Long
Session_I::sendSvcPrflRecord (const std::string& entityType, int recordId, int count,
    const CommUtil::DataRecord& svcRecord, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->relaySvcRecord (entityId, entityType, recordId, count, svcRecord);
}

Ice::Long
Session_I::sendToClient (const std::string& clientId, const std::string& entityType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToClient (entityId, clientId, entityType, msgType, msg);
}

Ice::Long
Session_I::sendToClientDataSeq (const std::string& clientId, const std::string& entityType,
    const std::string& msgType, const CommUtil::DataSeq& dataSeq, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToClientDataSeq (entityId, clientId, entityType, msgType, dataSeq);
}


Ice::Long
Session_I::sendToClientDataRec (const std::string& clientId, const std::string& entityType,
    const std::string& msgType, const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->sendToClientDataRec (entityId, clientId, entityType, msgType, dataRec);
}

Ice::Long
Session_I::sendToEntity (const std::string& srcEntityId, const std::string& destEntityId,
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
Session_I::sendToEntityDataSeq (const std::string& srcEntityId, const std::string& destEntityId,
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
Session_I::sendToEntityDataRec (const std::string& srcEntityId, const std::string& destEntityId,
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
Session_I::sendUpdate (const std::string& destId, const std::string& destType,
    const std::string& entityType, const std::string& msgType, const std::string& msg,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->handleUpdate (entityId, destId, destType, entityType, msgType, msg);
}

Ice::Long
Session_I::sendUpdateDataSeq (const std::string& destId, const std::string& destType,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return entityMonitor_Ptr->handleUpdateDataSeq (entityId, destId, destType, entityType, msgType,
      dataSeq);
}

Ice::Long
Session_I::ping (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync(mutex);

  common ();

  return entityMonitor_Ptr->ping (entityId);
}

void
Session_I::destroy (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  if (terminated)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "Session: " << "'" << entityId << "' " << "already terminated.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  try
    {
      entityMonitor_Ptr->deregisterEntity (entityId);
      curr.adapter->remove (curr.id);
    }
  catch (const Ice::ObjectAdapterDeactivatedException& ex)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "Session: " << "server is shutting down.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }
    }

  terminated = true;

  if (logFlag)
    {
      std::ostringstream oss;

      oss << "Session: " << "'" << entityId << "' is terminated.";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }
}

void
Session_I::common ()
{
  if (terminated)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "Session: " << "'" << entityId << "' ";
          oss << "attempted to send a message through a terminated session.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  if (!entityMonitor_Cb)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "Session: " << "'" << entityId << "' ";
          oss << "attempted to send a message without setting a callback.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw EntityComm::InvalidMessageException ("");
    }
}
