/*
 * entitycommunicator.cpp
 *
 *  Created on: 2012-01-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "entitycommunicator.h"

#include <assert.h>
#include <signal.h>

#include "entitycommunicator.h"
#include "./util/datarecord.h"
#include "./common/global.h"

EntityMonitorCallback_I::EntityMonitorCallback_I (EntityManager *em,
    const Ice::LoggerPtr& logger, const bool& flag) :
  entityManager (em),
  logger_Ptr (logger),
  logFlag (flag)
{}

EntityMonitorCallback_I::~EntityMonitorCallback_I ()
{}

void
EntityMonitorCallback_I::newClientMsg (Ice::Long timestamp, const std::string& clientId,
    const std::string& entityType, const std::string& msgType, const std::string& msg,
    const Ice::Current& curr)
{
  std::ostringstream oss;

  if (logFlag)
    {
      oss.str ("");
      oss << "EntityCommunicator: " << "new message from client: " << clientId;
      logger_Ptr->trace ("Info", oss.str ());
    }
/*
  std::cout << "clientId: " << clientId << std::endl;
  std::cout << "entityType: " << entityType << std::endl;
  std::cout << "msg: " << msg << std::endl;
*/
  if (entityType == ENTITY)
    {
      entityManager->addIncomingMsg (msg, CLIENT_MSG);

      if (logFlag)
        {
          oss.str ("");
          oss << "EntityCommunicator: " << "message from controlling client: " << clientId;
          logger_Ptr->trace ("Info", oss.str ());
        }
    }
}

void
EntityMonitorCallback_I::newClientMsgDataSeq (Ice::Long timestamp, const std::string& clientId,
    const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
    const Ice::Current& curr)
{
  std::vector <std::string> tmp;
  std::ostringstream oss;

  if (dataSeq.size () == 0)
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityCommunicator: " << "received empty data sequence from client: " << clientId;
          logger_Ptr->trace ("Warning", oss.str ());
        }

      return;
    }

  for (unsigned int i = 0; i < dataSeq.size (); ++i)
      tmp.push_back (dataSeq[i]);
  /// If it is a DATA sequence
  if (msgType == DATA)
      entityManager->addIncomingDataSeq (tmp, CLIENT_MSG);
  /// If is is a client TASK sequence
  else if (msgType == CLIENT_TASK)
      entityManager->addNewTask (tmp, CLIENT_MSG);
}

void
EntityMonitorCallback_I::newClientMsgDataRec (Ice::Long timestamp, const std::string& clientId,
    const std::string& entityType, const std::string& msgType,
    const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  Util::DataRecord dataRecord;
  std::ostringstream oss;

  if (dataRec.size () == 0)
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityCommunicator: " << "received empty data record from client: " << clientId;
          logger_Ptr->trace ("Warning", oss.str ());
        }
      return;
    }

  for (unsigned int i = 0; i < dataRec.size (); ++i)
    {
      CommUtil::DataSeq dataSeq;

      dataSeq = dataRec[i];
      dataRecord.dataList.push_back (dataSeq);
    }

  if (msgType == CLIENT_COLLABORATIVE_TASK)
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityCommunicator: " << "received a collaborative task from client: " << clientId;
          logger_Ptr->trace ("Warning", oss.str ());
        }
      /// Add the collaborative task to the queue and notify the entity manager
      entityManager->addNewCollaborativeTask (dataRecord, CLIENT_MSG);
      entityManager->toggleCollaborationMode (true);
    }
  else if (msgType == DATA)
    {
      entityManager->addIncomingDataRec (dataRecord, CLIENT_MSG);
    }
}

void
EntityMonitorCallback_I::newUpdateMsg (Ice::Long timestamp, const std::string& srcId,
    const std::string& srcType, const std::string& destType, const std::string& msgType,
    const std::string& msg, const Ice::Current& curr)
{
  std::ostringstream oss;
  /// If it is a client-related update
  if (srcType == CLIENT && destType == ENTITY)
    {
      /// If it is a request message
      if (msgType == REQUEST)
        {
          if (msg == CHECKOUT_ENTITY)
            {
              /// Signal the entity manager to compile and dispatch service records
              entityManager->setRequested (true);
              entityManager->setClientUuid (srcId);
              if (logFlag)
                {
                  oss << "EntityCommunicator: " << "check-out request from client: " << srcId;
                  logger_Ptr->trace ("Info", oss.str ());
                }
            }
        }
      /// If it is an ALERT messages
      else if (msgType == ALERT)
        {
          /// If it is a signal that the controlling client released entity or abruptly disconnected
          if (msg == CLIENT_UNREGISTERED || msg == RELEASE_ENTITY)
            {
              entityManager->setReleased (true);
              entityManager->setClientUuid ("");
            }
          /// If it is a release alert
          if (msg == RELEASE_ENTITY)
            {
              if (logFlag)
                {
                  oss << "EntityCommunicator: " << "controlling client: " << srcId;
                  oss << " released us.";
                  logger_Ptr->trace ("Alert", oss.str ());
                }
            }
          /// If it is a client-disconnection alert
          else if (msg == CLIENT_UNREGISTERED)
            {
              if (logFlag)
                {
                  oss << "EntityCommunicator: " << "controlling client: " << srcId;
                  oss << " abruptly disconnected.";
                  logger_Ptr->trace ("Alert", oss.str ());
                }
            }
        }
    }
  /// If it is an environment-related update
  else if (srcType == ENTITY_SERVER && destType == ENTITY)
    {
      if (msgType == INFO)
        {
          /// If registration with environment is complete
          if (msg == REGISTERED)
            {
              entityManager->setConnected (true);
              if (logFlag)
                {
                  oss << "EntityCommunicator: " << "connected to environment.";
                  logger_Ptr->trace ("Info", oss.str ());
                }
            }
          /// If last messages was received and processed
          else if (msg == RECEIVED)
            {
              if (logFlag)
                {
                  oss << "EntityCommunicator: " << "registered successfully.";
                  logger_Ptr->trace ("Info", oss.str ());
                }
            }
        }
      else if (msgType == ALERT)
        {
          if (entityManager->getInitiator () && msg == ENTITY_RELEASED)
              entityManager->setInitiator (false);
          else
              entityManager->addIncomingMsg (msg, ENVIR_MSG);
        }
      else if (msgType == ERROR)
        {
          /// If last messages was not properly formated
          if (msg == BAD_MSG)
            {
              if (logFlag)
                {
                  oss << "EntityCommunicator: " << "last sent messages was not valid or corrupted.";
                  logger_Ptr->trace ("Error", oss.str ());
                }
            }
        }
    }
}

void
EntityMonitorCallback_I::newEntityMsg (Ice::Long timestamp, const std::string& srcEntityId,
    const std::string& destEntityId, const std::string& clientId,
    const std::string& srcEntityType, const std::string& destEntityType,
    const std::string& clientType, const std::string& msgType, const std::string& msg,
    const Ice::Current& curr)
{
  std::ostringstream oss;

  if (logFlag)
    {
      oss << "EntityCommunicator: " << "message from entity: '" << srcEntityId << "'";
      logger_Ptr->trace ("Info", oss.str ());
      std::cout << "MSG: " << msg << std::endl;
    }
  /// Authenticate entity
  if (destEntityId == entityManager->getUuid () && clientId == entityManager->getClientUuid ()
      && srcEntityType == ENTITY && destEntityType == ENTITY)
    {
      /// If it is a request message
      if (msgType == REQUEST)
        {
          /// If it is a collaboration request toggle the slave mode and reply to originating entity
          if (msg == COLLABORATION)
            {
              entityManager->setCollaboratorUuid (srcEntityId);
              entityManager->toggleSlaveMode (true);
              entityManager->addOutgoingEntityMsg (COLLABORATION_OK, INFO, srcEntityId);
            }
        }
      else if (msgType == INFO)
        {
          entityManager->addIncomingMsg (msg, ENTITY_MSG);
        }
    }
  else
    {
      if (logFlag)
        {
          oss << "EntityCommunicator: " << "message from an entity under different control.";
          logger_Ptr->trace ("Warning", oss.str ());
        }
    }
}

void
EntityMonitorCallback_I::newEntityMsgDataSeq (Ice::Long timestamp, const std::string& srcEntityId,
    const std::string& destEntityId, const std::string& clientId,
    const std::string& srcEntityType, const std::string& destEntityType,
    const std::string& clientType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
    const Ice::Current& curr)
{
  std::ostringstream oss;
  std::vector <std::string> tmp;

  if (logFlag)
    {
      oss << "EntityCommunicator: " << "data sequence from entity: '" << srcEntityId << "'";
      logger_Ptr->trace ("Info", oss.str ());
    }
  /// Authenticate entity
  if (clientId == entityManager->getClientUuid () && srcEntityType == ENTITY
      && destEntityType == ENTITY)
    {
      /// If it is a task from another entity
      if (msgType == TSK)
        {
          for (unsigned int i = 0; i < dataSeq.size (); ++i)
              tmp.push_back (dataSeq[i]);

          entityManager->addNewTask (tmp, ENTITY_MSG);
        }
    }
  else
    {
      if (logFlag)
        {
          oss << "EntityCommunicator: " << "message from an entity under different control.";
          logger_Ptr->trace ("Warning", oss.str ());
        }
    }
}

void
EntityMonitorCallback_I::newEntityMsgDataRec (Ice::Long timestamp, const std::string& srcEntityId,
    const std::string& destEntityId, const std::string& clientId,
    const std::string& srcEntityType, const std::string& destEntityType,
    const std::string& clientType, const std::string& msgType,
    const CommUtil::DataRecord& dataRec, const Ice::Current& curr)
{
  std::ostringstream oss;

  if (logFlag)
    {
      oss << "EntityCommunicator: " << "data record from entity: '" << srcEntityId << "'";
      logger_Ptr->trace ("Info", oss.str ());
    }
  /// TODO
}

void
EntityMonitorCallback_I::stopSignal (Ice::Long timestamp, const std::string& clientId,
    const std::string& entityType, const std::string& msg, const Ice::Current& curr)
{
  std::ostringstream oss;

  if (logFlag)
    {
      oss << "EntityCommunicator: " << "stop signal.";
      logger_Ptr->trace ("Info", oss.str ());
    }
  /// Toggle stop
  entityManager->toggleStop (true, msg);
}

void
EntityMonitorCallback_I::clearSignal (Ice::Long timestamp, const std::string& clientId,
    const std::string& entityType, const std::string& msg, const Ice::Current& curr)
{
  std::ostringstream oss;

  if (logFlag)
    {
      oss << "EntityCommunicator: " << "clear signal.";
      logger_Ptr->trace ("Info", oss.str ());
    }
  /// Clear all pending tasks
  entityManager->clearPendingClientTasks ();
}

void
EntityMonitorCallback_I::resetSignal (Ice::Long timestamp, const std::string& clientId,
    const std::string& entityType, const std::string& msg, const Ice::Current& curr)
{
  std::ostringstream oss;

  if (logFlag)
    {
      oss << "EntityCommunicator: " << "reset signal.";
      logger_Ptr->trace ("Info", oss.str ());
    }
  /// Clear all pending tasks and data queues
  entityManager->clearPendingClientTasks ();
  entityManager->clearAllData ();
  /// Toggle stop
  entityManager->toggleStop (true, msg);
}

void
EntityMonitorCallback_I::pong (Ice::Long timestamp, const Ice::Current& curr)
{}

void
EntityMonitorCallback_I::disconnect (Ice::Long timestamp, const Ice::Current& curr)
{}

/******************************************************/
/******************************************************/

StateMachine::StateMachine (EntityManager *em) :
  entityManager (em)
{}

StateMachine::~StateMachine ()
{}

void
StateMachine::runTimerTask ()
{
  entityManager->exec ();
}

/******************************************************/
/******************************************************/

EntityCommunicator::EntityCommunicator (bool flag) :
  Application(Ice::NoSignalHandling),
  initFlag (true),
  logFlag (flag),
  active (false)
{}

EntityCommunicator::~EntityCommunicator ()
{}

bool
EntityCommunicator::init ()
{
  uuid = IceUtil::generateUUID () ;
  entityManager = new EntityManager (logger_Ptr, logFlag);
  stateMachine_Ptr = new StateMachine (entityManager);
  timer_Ptr = new IceUtil::Timer ();

  if (!entityManager->init (uuid))
      return false;

  timer_Ptr->scheduleRepeated (stateMachine_Ptr, IceUtil::Time::milliSeconds (50));

  return true;
}

Glacier2::SessionPrx
EntityCommunicator::createSession ()
{
  Glacier2::SessionPrx session;
  std::string msg;
  std::ostringstream oss;

  if (initFlag)
    {
      logger_Ptr = communicator ()->getLogger ();

      if (!init ())
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityCommunicator: " << "Failed to initialize. Exiting...";
              logger_Ptr->trace ("Info", oss.str ());
            }

          exit (0);
        }
      initFlag  = false;
    }

  compileSimpleMsg (&msg, INIT);
  active = false;

  if (logFlag)
    {
      oss.str ("");
      oss << "EntityCommunicator: " << "Connecting to environment...";
      logger_Ptr->trace ("Info", oss.str ());
    }

  while (true)
    {
      /// If the permission to connect to environment is not yet given keep trying
      if (!entityManager->getConnectSignal ())
        {
          sleep (1);
          continue;
        }

      try
        {
          session = router ()->createSession (msg, "");
          break;
        }
      catch (const Glacier2::CannotCreateSessionException& ex)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityCommunicator: " << "Failed to create session.";
              logger_Ptr->trace ("Info", oss.str ());
            }
        }
      catch (const Glacier2::PermissionDeniedException& ex)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityCommunicator: " << "Failed to create session. Permission denied.";
              logger_Ptr->trace ("Info", oss.str ());
            }
        }
      catch (const Ice::LocalException& ex)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityCommunicator: " << "Failed to connect to server.";
              logger_Ptr->trace ("Info", oss.str ());
            }
        }

      if (logFlag)
        {
          oss.str ("");
          oss << "EntityCommunicator: " << "Retrying...";
          logger_Ptr->trace ("Info", oss.str ());
        }
      sleep (1);
    }

  return session;
}

void
EntityCommunicator::sessionDestroyed ()
{
  entityManager->setConnected (false);
  if (logFlag)
    {
      std::ostringstream oss;

      oss << "EntityCommunicator: " << "session destroyed.";
      logger_Ptr->trace ("Info", oss.str ());
    }
}

int
EntityCommunicator::runWithSession (int argc, char *argv[])
{
  std::ostringstream oss;
  std::string tmp;

  if (argc > 1)
      throw std::string ("Usage: ") + appName();

  activeSession = EntityComm::SessionPrx::uncheckedCast (this->session ());
  activeSession->setCallback (EntityComm::EntityMonitorCallbackPrx::uncheckedCast \
      (addWithUUID (new EntityMonitorCallback_I (entityManager, logger_Ptr, logFlag))));
  active = true;

  if (logFlag)
    {
      oss.str ("");
      oss << "EntityCommunicator: " << "Started new session.";
      logger_Ptr->trace ("Info", oss.str ());
    }
  /// Send the basic profile to the environment
  entityManager->getBasisPrflData (&tmp);
  activeSession->begin_sendBasicPrfl (ENTITY, tmp);
  /// Keep waiting for ongoing messages to send to the environment
  while (true)
    {
      std::string msg;
      std::string msgType;
      CommUtil::DataSeq dataSeq;
      Util::DataRecord svcDataRecord;
      CommUtil::DataRecord dataRecord;
      std::vector <std::string> out;
      std::string destId;
      unsigned int tmp0;
      unsigned int tmp1;

      /// If connection was lost throw an exception to try to create a new session
      if (!entityManager->getEntityOnline ())
        {
          entityManager->setClientUuid ("");
          throw Ice::ConnectionLostException ("Connection to environment lost", 1);
        }

      /// If the entity was request start dispatching the service records to the client
      tmp0 = entityManager->getSvcPrflCount ();
      if (tmp0 > 0)
        {
          tmp1 = entityManager->getCounter ();

          if (entityManager->getSvcDataRecord (&svcDataRecord))
            {
              compileCommDataRecord (&svcDataRecord, &dataRecord);
              activeSession->begin_sendSvcPrflRecord (ENTITY, tmp1, tmp0, dataRecord);
            }
        }
      /// Scanning outgoing data sequences queues
      if (entityManager->getOutgoingDataSeq (&out, &msgType, CLIENT_MSG))
        {
          CommUtil::DataSeq dataSeq;

          for (unsigned int i = 0; i < out.size (); ++i)
              dataSeq.push_back (out[i]);

          activeSession->sendToClientDataSeq (entityManager->getClientUuid (), ENTITY, msgType,
              dataSeq);
        }
      /// Scanning outgoing messages queues
      if (entityManager->getOutgoingMsg (&msg, &msgType, CLIENT_MSG))
        {
          activeSession->begin_sendToClient (entityManager->getClientUuid (), ENTITY, msgType, msg);
        }
      /// A request message is on the queue for the environment
      if (entityManager->getOutgoingMsg (&msg, &msgType, ENVIR_MSG))
        {
          activeSession->begin_sendUpdate ("", ENTITY_SERVER, ENTITY, msgType, msg);
        }
      /// A data sequence for the environment
      if (entityManager->getOutgoingDataSeq (&dataSeq, &msgType, ENVIR_MSG))
        {
          activeSession->begin_sendUpdateDataSeq ("", ENTITY_SERVER, ENTITY, msgType, dataSeq);
        }
      if (entityManager->getOutgoingEntityMsg (&msg, &msgType, &destId))
        {
          activeSession->begin_sendToEntity (uuid, destId, entityManager->getClientUuid(), ENTITY,
              ENTITY, CLIENT, msgType, msg);
        }
      if (entityManager->getOutgoingEntityDataSeq (&dataSeq, &msgType, &destId))
        {
          activeSession->begin_sendToEntityDataSeq (uuid, destId, entityManager->getClientUuid(),
              ENTITY, ENTITY, CLIENT, msgType, dataSeq);
        }
      usleep (500);
    }

  return EXIT_SUCCESS;
}

void
EntityCommunicator::compileSimpleMsg (std::string *msg, const std::string& msgBody)
{
  msg->append (ENTITY);
  msg->append (DELIM);
  msg->append (uuid);

  if (msgBody.length () > 0)
    {
      msg->append (DELIM);
      msg->append (msgBody);
    }
}

void
EntityCommunicator::compileCommDataRecord (Util::DataRecord *svcDataRecord,
    CommUtil::DataRecord *dataRecord)
{
  for (unsigned int i = 0; i < svcDataRecord->dataList.size (); ++i)
    {
      CommUtil::DataSeq dataSeq;

      for (unsigned int j = 0; j < svcDataRecord->dataList[i].size (); ++j)
          dataSeq.push_back (svcDataRecord->dataList[i][j]);

      dataRecord->push_back (dataSeq);
    }
}


/*****************
 * MAIN
 ****************/
int
main(int argc, char* argv[])
{
/*
  Ice::InitializationData initData;
  initData.properties = Ice::createProperties ();
*/
  EntityCommunicator entityCommunicator (true);

  return entityCommunicator.main (argc, argv, CFG_NET_FILE.c_str ());
}
