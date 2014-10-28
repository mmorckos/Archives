/*
 * entitycommunicator.h
 *
 *  Created on: 2012-01-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef ENTITYCOMMUNICATOR_H_
#define ENTITYCOMMUNICATOR_H_

#include <Ice/Ice.h>
#include <Glacier2/Glacier2.h>
#include <IceUtil/IceUtil.h>

#include "entitymanager.h"
#include "entitycomm.h"

const std::string CFG_NET_FILE = "../config/entitygl2client.cfg";

class EntityMonitorCallback_I : public EntityComm::EntityMonitorCallback
{
public:
  EntityMonitorCallback_I (EntityManager *em, const Ice::LoggerPtr& logger, const bool& flag);
  virtual ~EntityMonitorCallback_I ();

  /*! \brief Session callback for client messages.
   *
   * \param timestamp Timestamp.
   * \param clientId Client UUID.
   * \param entityType Entity type (for authentication).
   * \param msgType Message type.
   * \param msg Message.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa newClientMsgDataSeq ()
   * \sa newClientMsgDataRec ()
   */
  void newClientMsg (Ice::Long timestamp, const std::string& clientId,
      const std::string& entityType, const std::string& msgType, const std::string& msg,
      const Ice::Current& curr);

  /*! \brief Session callback for client data sequences.
   *
   * \param timestamp Timestamp.
   * \param clientId Client UUID.
   * \param entityType Entity type (for authentication).
   * \param msgType Message type.
   * \param dataSeq Data sequence.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa newClientMsg ()
   * \sa newClientMsgDataRec ()
   */
  void newClientMsgDataSeq (Ice::Long timestamp, const std::string& clientId,
      const std::string& entityType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
      const Ice::Current& curr);

  /*! \brief Session callback for client data records.
   *
   * \param timestamp Timestamp.
   * \param clientId Client UUID.
   * \param entityType Entity type (for authentication).
   * \param msgType Message type.
   * \param dataRec Data record.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa newClientMsg ()
   * \sa newClientMsgDataSeq ()
   */
  void newClientMsgDataRec (Ice::Long timestamp, const std::string& clientId,
      const std::string& entityType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec, const Ice::Current& curr);

  /*! \brief Session callback for update messages from an external actor (the server).
   *
   * \param timestamp Timestamp.
   * \param srcId Source UUID.
   * \param srcType Source type (for authentication).
   * \param destType Destination type (for authentication)..
   * \param msgType Message type.
   * \param msg Message.
   * \param curr Houskeeping information for the executing request.
   */
  void newUpdateMsg (Ice::Long timestamp, const std::string& srcId,
      const std::string& srcType, const std::string& destType, const std::string& msgType,
      const std::string& msg, const Ice::Current& curr);

  /*! \brief Session callback for a message coming from another entity.
   *
   * \param timestamp Timestamp.
   * \param srcEntityId Source entity UUID (for authentication).
   * \param destEntityId Destination entity UUID (for authentication).
   * \param clientId Controlling client UUID (for authentication)..
   * \param srcEntityType Source entity type.
   * \param destEntityType Destination entity type.
   * \param clientType Client type.
   * \param msgType Message type.
   * \para msg Message.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa newEntityMsgDataSeq ()
   * \sa newEntityMsgDataRec ()
   */
  void newEntityMsg (Ice::Long timestamp, const std::string& srcEntityId,
      const std::string& destEntityId, const std::string& clientId,
      const std::string& srcEntityType, const std::string& destEntityType,
      const std::string& clientType, const std::string& msgType, const std::string& msg,
      const Ice::Current& curr);

  /*! \brief Session callback for data sequences coming from another entity.
   *
   * \param timestamp Timestamp.
   * \param srcEntityId Source entity UUID (for authentication).
   * \param destEntityId Destination entity UUID (for authentication).
   * \param clientId Controlling client UUID (for authentication)..
   * \param srcEntityType Source entity type.
   * \param destEntityType Destination entity type.
   * \param clientType Client type.
   * \param msgType Message type.
   * \para dataSeq Data sequence.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa newEntityMsg ()
   * \sa newEntityMsgDataRec ()
   */
  void newEntityMsgDataSeq (Ice::Long timestamp, const std::string& srcEntityId,
      const std::string& destEntityId, const std::string& clientId,
      const std::string& srcEntityType, const std::string& destEntityType,
      const std::string& clientType, const std::string& msgType, const CommUtil::DataSeq& dataSeq,
      const Ice::Current& curr);

  /*! \brief Session callback for data records coming from another entity.
   *
   * \param timestamp Timestamp.
   * \param srcEntityId Source entity UUID (for authentication).
   * \param destEntityId Destination entity UUID (for authentication).
   * \param clientId Controlling client UUID (for authentication)..
   * \param srcEntityType Source entity type.
   * \param destEntityType Destination entity type.
   * \param clientType Client type.
   * \param msgType Message type.
   * \para dataRec Data record.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa newEntityMsg ()
   * \sa newEntityMsgDataSeq   ()
   */
  void newEntityMsgDataRec (Ice::Long timestamp, const std::string& srcEntityId,
      const std::string& destEntityId, const std::string& clientId,
      const std::string& srcEntityType, const std::string& destEntityType,
      const std::string& clientType, const std::string& msgType,
      const CommUtil::DataRecord& dataRec, const Ice::Current& curr);

  /*! \brief Session callback for a stop command from the client.
   *
   * \param timestamp Timestamp.
   * \param clientId Client UUID (for authentication).
   * \param entityType Entity type (for authentication).
   * \param msg Accompanying message.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa clearSignal ()
   * \sa resetSignal ()
   */
  void stopSignal (Ice::Long timestamp, const std::string& clientId, const std::string& entityType,
      const std::string& msg, const Ice::Current& curr);

  /*! \brief Session callback for a clear command from the client.
   *
   * \param timestamp Timestamp.
   * \param clientId Client UUID (for authentication).
   * \param entityType Entity type (for authentication).
   * \param msg Accompanying message.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa stopSignal ()
   * \sa resetSignal ()
   */
  void clearSignal (Ice::Long timestamp, const std::string& clientId, const std::string& entityType,
      const std::string& msg, const Ice::Current& curr);

  /*! \brief Session callback for a reset command from the client.
   *
   * \param timestamp Timestamp.
   * \param clientId Client UUID (for authentication).
   * \param entityType Entity type (for authentication).
   * \param msg Accompanying message.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa stopSignal ()
   * \sa clearSignal ()
   */
  void resetSignal (Ice::Long timestamp, const std::string& clientId, const std::string& entityType,
      const std::string& msg, const Ice::Current& curr);

  /*! \brief Auxiliary session callback for testing.
   *
   * \param timestamp Timestamp.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa disconnect ()
   */
  void pong (Ice::Long timestamp, const Ice::Current& curr);

  /*! \brief Auxiliary session callback for testing.
   *
   * \param timestamp Timestamp.
   * \param curr Houskeeping information for the executing request.
   *
   * \sa pong ()
   */
  void disconnect (Ice::Long timestamp, const Ice::Current& curr);

protected:
  EntityManager *entityManager;       /*!< Pointer to EntityManager object. */
  const Ice::LoggerPtr logger_Ptr;    /*!< Ice logging utility. */
  const bool logFlag;                 /*!< Flag to toggle logging. */
};

/******************************************************/
/******************* StateMachine *********************/
/******************************************************/

class StateMachine : public IceUtil::TimerTask
{

public:
  StateMachine (EntityManager *em);
  virtual ~StateMachine ();

  virtual void runTimerTask ();

protected:
  EntityManager *entityManager;       /*!< Pointer to EntityManager object. */
};
typedef IceUtil::Handle <StateMachine> StateMachinePtr;

/******************************************************/
/**************** EntityCommunicator ******************/
/******************************************************/

class EntityCommunicator : public Glacier2::Application
{

public:
  /*! \brief Constructor.
   *
   * \param flag Logging flag.
   */
  EntityCommunicator (bool flag = false);

  /*! \brief Destructor.
   */
  virtual ~EntityCommunicator ();

  /*! \brief This function creates and initializes the entity manager and the state machine.
   * The state machine runs in a separate thread.
   *
   * \return true if initialization is success. false otherwise.
   */
  bool init ();

  /*! \brief This function creates and initializes a Glacier2-based session with the server.
   *
   * \return Glacier2 session proxy object.
   */
  virtual Glacier2::SessionPrx createSession ();

  /*! \brief This function is callback that handles clean-up or re-initializes in case the sessions
   * times out or is destroyed
   */
  virtual void sessionDestroyed ();

  /*! \brief This function houses the main routine of the communicator.
   *
   * The communicator periodically checks outgoing queues and transmits data and information.
   *
   * \param argc Parameters count.
   * \param argv List of parameters.
   *
   * \return Termination status.
   */
  virtual int runWithSession (int argc, char *argv[]);

protected:
  bool initFlag;                      /*!< Flag to signal if this is the first session to create. */
  EntityManager *entityManager;         /*!< Pointer to EntityManager object. */
  StateMachinePtr stateMachine_Ptr;     /*!< Pointer to StateMachine object. */
  EntityComm::SessionPrx activeSession; /*!< Session proxy to communicate with the server. */
  IceUtil::TimerPtr timer_Ptr;          /*!< Task timer to run the state machine. */
  Ice::LoggerPtr logger_Ptr;            /*!< Ice logging utility. */
  const bool logFlag;                   /*!< Flag to toggle logging. */
  std::string uuid;                     /*!< Entity UUID. */
  bool active;                          /*!< Flag indicating if connection with server is active. */
  IceUtil::Mutex mutex;
  std::string currMsg;

  /*! \brief Packs a message into a specified format (for transmission).
   *
   * \param msg Final message to be produced.
   * \param msgBody Message body.
   *
   * \sa compileCommDataRecord ()
   */
  void compileSimpleMsg (std::string *msg, const std::string& msgBody);

  /*! \brief Packs a data record into a specified format (for transmission).
   *
   * \param dataRecord Data record.
   * \param dataRecord Data record to be produced in the proper format.
   *
   * \sa compileSimpleMsg ()
   */
  void compileCommDataRecord (Util::DataRecord *svcDataRecord, CommUtil::DataRecord *dataRecord);
};

#endif /* ENTITYCOMMUNICATOR_H_ */
