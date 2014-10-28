/*
 * session_i.cpp
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "session_i.h"

class SessionCallBackAdapter : public ClientMonitorCallbackAdapter
{

public:
  SessionCallBackAdapter (
      const FrontEndComm::ClientMonitorCallbackPrx& cb,
      const FrontEndComm::SessionPrx& session, const std::string& id,
      const Ice::LoggerPtr& logger, const bool flag) :
        clientMonitorCb_prx (cb),
        session_prx (session),
        clientId (id),
        iceLogger_Ptr (logger),
        logFlag (flag)
  {}

  void newBasicPrflMsg (const FrontEndComm::NewBasicPrflMsgEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_newBasicPrflMsgPtr newBasicPrflMsgCb =
        FrontEndComm::newCallback_ClientMonitorCallback_newBasicPrflMsg (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_newBasicPrflMsg (ev->timestamp, ev->entityId, ev->entityType,
            ev->msg, newBasicPrflMsgCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newSvcRecordMsg (const FrontEndComm::NewSvcRecordMsgEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_newSvcRecordMsgPtr newSvcRecordMsgCb =
        FrontEndComm::newCallback_ClientMonitorCallback_newSvcRecordMsg (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_newSvcRecordMsg (ev->timestamp, ev->entityId, ev->entityType,
            ev->recordId, ev->count, ev->svcRecord, newSvcRecordMsgCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newEntityMsg (const FrontEndComm::NewEntityMsgEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_newEntityMsgPtr newEntityMsgCb =
        FrontEndComm::newCallback_ClientMonitorCallback_newEntityMsg (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_newEntityMsg (ev->timestamp, ev->entityId, ev->entityType,
            ev->msgType, ev->msg, newEntityMsgCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newEntityMsgDataSeq (const FrontEndComm::NewEntityMsgDataSeqEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_newEntityMsgDataSeqPtr newEntityMsgDataSeqCb =
        FrontEndComm::newCallback_ClientMonitorCallback_newEntityMsgDataSeq (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_newEntityMsgDataSeq (ev->timestamp, ev->entityId,
            ev->entityType, ev->msgType, ev->dataSeq, newEntityMsgDataSeqCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newEntityMsgDataRec (const FrontEndComm::NewEntityMsgDataRecEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_newEntityMsgDataRecPtr newEntityMsgDataRecCb =
        FrontEndComm::newCallback_ClientMonitorCallback_newEntityMsgDataRec (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_newEntityMsgDataRec (ev->timestamp, ev->entityId,
            ev->entityType, ev->msgType, ev->dataRec, newEntityMsgDataRecCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newUpdateMsg (const FrontEndComm::NewUpdateMsgEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_newUpdateMsgPtr newUpdateMsgCb =
        FrontEndComm::newCallback_ClientMonitorCallback_newUpdateMsg (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_newUpdateMsg (ev->timestamp, ev->sourceId, ev->sourceType,
            ev->msgType, ev->msg, newUpdateMsgCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void newMsgDataSeq (const FrontEndComm::NewMsgDataSeqEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_newMsgDataSeqPtr newMsgDataSeqCb =
        FrontEndComm::newCallback_ClientMonitorCallback_newMsgDataSeq (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_newMsgDataSeq (ev->timestamp, ev->sourceId, ev->sourceType,
            ev->msgType, ev->dataSeq, newMsgDataSeqCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void pong (const FrontEndComm::PongEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_pongPtr pongCb =
        FrontEndComm::newCallback_ClientMonitorCallback_pong (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_pong (ev->timestamp, pongCb);
      }
    catch (const Ice::CommunicatorDestroyedException& ex)
      {}
  }

  void disconnect (const FrontEndComm::DisconnectEventPtr& ev)
  {
    FrontEndComm::Callback_ClientMonitorCallback_disconnectPtr disconnectCb =
        FrontEndComm::newCallback_ClientMonitorCallback_disconnect (this,
            &SessionCallBackAdapter::success, &SessionCallBackAdapter::failure);
    try
      {
        clientMonitorCb_prx->begin_disconnect (ev->timestamp, disconnectCb);
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

        oss << "SessionCallBackAdapter: '" << clientId << "' will be terminated.";
        iceLogger_Ptr->trace ("Error", oss.str ());
      }

    try
      {
        /// Omitted due to potential deadlock occurrence
//        session_prx->destroy ();
      }
    catch (const Ice::LocalException& ex)
    {}
  }

protected:
  const FrontEndComm::ClientMonitorCallbackPrx clientMonitorCb_prx;
  const FrontEndComm::SessionPrx session_prx;
  const std::string clientId;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;
};

/******************************************************/
/******************************************************/

Session_I::Session_I (const ClientMonitorPtr& cm, const std::string& id,
    const Ice::LoggerPtr& logger, const bool flag) :
  clientMonitor_Ptr (cm),
  clientId (id),
  iceLogger_Ptr (logger),
  logFlag (flag),
  terminated (false)
{}

Session_I::~Session_I ()
{}

void
Session_I::setCallback (const FrontEndComm::ClientMonitorCallbackPrx& callback,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  Ice::Context ctx;

  if (terminated)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "Session: " << "'" << clientId << "' ";
          oss << "terminated. Cannot set session callback.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  if (clientMonitor_Cb || !callback)
      return;

  ctx["_fwd"] = "o";
  clientMonitor_Cb = new SessionCallBackAdapter (callback->ice_context (ctx),
      FrontEndComm::SessionPrx::uncheckedCast (curr.adapter->createProxy (curr.id)), clientId,
      iceLogger_Ptr, logFlag);

  clientMonitor_Ptr->registerClient (clientId, this->clientMonitor_Cb);
}

Ice::Long
Session_I::sendToEntity (const std::string& entityId, const std::string& entityType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->sendToEntity (clientId, entityId, entityType, msgType, msg);
}

Ice::Long
Session_I::sendToEntityDataSeq (const std::string& entityId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
    const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->sendToEntityDataSeq (clientId, entityId, entityType, msgType, dataSeq);
}

Ice::Long
Session_I::sendToEntityDataRec (const std::string& entityId,
    const std::string& entityType, const std::string& msgType,
    const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->sendToEntityDataRec (clientId, entityId, entityType, msgType, dataRec);
}

Ice::Long
Session_I::sendRequest (const std::string& destId, const std::string& destType,
    const std::string& msgType, const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->handleRequest (clientId, destId, destType, msgType, msg);
}

Ice::Long
Session_I::sendStopSignal (const std::string& entityId, const std::string& entityType,
    const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->relayStopSignal (clientId, entityId, entityType, msg);
}

Ice::Long
Session_I::sendClearSignal (const std::string& entityId, const std::string& entityType,
    const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->relayClearSignal (clientId, entityId, entityType, msg);
}

Ice::Long
Session_I::sendResetSignal (const std::string& entityId, const std::string& entityType,
    const std::string& msg, const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->relayResetSignal (clientId, entityId, entityType, msg);
}

Ice::Long
Session_I::ping (const Ice::Current& curr)
{
  IceUtil::Mutex::Lock sync (mutex);

  common ();

  return clientMonitor_Ptr->ping (clientId);
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

          oss << "Session: " << "'" << clientId << "' " << "already terminated.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  try
    {
      clientMonitor_Ptr->deregisterClient (clientId);
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

      oss << "Session: " << "'" << clientId << "' is terminated.";
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

          oss << "Session: " << "'" << clientId << "' ";
          oss << "attempted to send a message through a terminated session.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw Ice::ObjectNotExistException (__FILE__, __LINE__);
    }

  if (!clientMonitor_Cb)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "Session: " << "'" << clientId << "' ";
          oss << "attempted to send a message without setting a callback.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw FrontEndComm::InvalidMessageException ("");
    }
}
