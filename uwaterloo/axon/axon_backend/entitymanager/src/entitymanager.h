/*
 * entitymanager.h
 *
 *  Created on: 2011-12-03
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef ENTITYMANAGER_H_
#define ENTITYMANAGER_H_

#include <iostream>
#include <list>
#include <queue>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "backendinterfacer.h"
#include "serviceprofiler.h"
#include "./util/datarecord.h"
#include "./protobufdata/entityprofile.pb.h"

class State;

class EntityManager
{

public:
  BackEndInterfacer backendInterfacer;
  ServiceProfiler serviceProfiler;

  /*! \brief Constructor.
   *
   * \param logger Reference to the logging object.
   * \param flag Logging flag.
   */
  EntityManager (const Ice::LoggerPtr& logger, const bool& flag);

  /*! \brief Destructor.
   */
  ~EntityManager ();

  /*! \brief Initialze the manager.
   *
   * Initialization involves loading basic configuration parameters.
   *
   * \param uuid Entity UUID.
   * \return returns true if initialization is success. false otherwise.
   */
  bool init (std::string uuid);

  /*! \brief Executes the current state.
   */
  void exec ();

  /*! \brief Initializes connection with the entity.
   *
   * \return returns true if initialization is success. false otherwise.
   */
  bool openEntityConn ();

  /*! \brief Closes socket connection with the entity.
   */
  void resetEntityConn ();

  /*! \brief Checks the current status of the entity.
   *
   * \return returns true if operation is success. false if connection with entity is lost.
   */
  bool probeStatus (std::string *status);

  /*! \brief Sets the status of entity connection.
   *
   * \param flag Connection status.
   */
  void setEntityOnline (const bool& flag);

  /*! \brief Returns the status of entity connection.
   *
   * \return Connection status.
   */
  bool getEntityOnline ();

  /*! \brief Sets the status of server connection.
   *
   * \param flag Connection status.
   */
  void setConnected (const bool& flag);

  /*! \brief Returns the status of server connection.
   *
   * \return Connection status.
   */
  bool getConnected ();

  /*! \brief Sets permission to establish connection with the server.
   *
   * \param Connection permission.
   */
  void setConnectSignal (const bool& flag);

  /*! \brief Returns the current permission to establish connection with the server.
   *
   * \return Permission.
   */
  bool getConnectSignal ();

  /*! \brief Signals whether there is a pending check-out request or not.
   *
   * \param flag Permission.
   */
  void setRequested (const bool& flag);

  /*! \brief Checks if there is a pending check-out request.
   *
   * \return Request status.
   */
  bool getRequested ();

  /*! \brief Signals that entity is released.
   *
   * \param flag Release status.
   */
  void setReleased (const bool& flag);

  /*! \brief Checks whether the entity was released or not.
   *
   * \return Release status.
   */
  bool getReleased ();

  /*! \brief Sets the checked-out status of the entity.
   *
   * \param Checked-out status.
   */
  void setCheckedOut (const bool& flag);

  /*! \brief Gets the checked-out status of the entity.
   *
   * \return Checked-out status.
   */
  bool getCheckedOut ();

  /*! \brief Toggles collaboration mode.
   *
   * \param Collaboration mode.
   */
  void toggleCollaborationMode (const bool& flag);

  /*! \brief Checks if entity is in collaboration mode.
   *
   * \return Collaboration mode.
   */
  bool collaborationMode ();

  /*! \brief Toggles slave mode.
   *
   * \param Slave mode.
   */
  void toggleSlaveMode (const bool& flag);

  /*! \brief Checks if entity is in slave mode.
   *
   * \return Slave mode.
   */
  bool slaveMode ();

  /*! \brief Toggles stop signal.
   *
   * \param flag Stop indicator.
   * \param msg Accompanying message.
   */
  void toggleStop (const bool& flag, const std::string& msg = "");

  /*! \brief Checks if there is a pending stop signal.
   *
   * \param msg Pointer to accompanying message.
   * \return Stop signal.
   */
  bool stopSignal (std::string *msg);

  /*! \brief Sets whether the entity is the initiator of a collaboration mode.
   *
   * \param flag Is entity initiator.
   */
  ///FIXME Temporary fix
  void setInitiator (const bool& flag);

  /*! \brief Checks if the entity is the initiator of a collaboration mode.
   *
   * \return Is entity the initiator.
   */
  ///FIXME Temporary fix
  bool getInitiator ();

  /*! \brief Sets the UUID of the current controlling client.
   *
   * \param uuid Client UUID.
   */
  void setClientUuid (const std::string& uuid);

  /*! \brief Returns the UUID of the current controlling client.
   *
   * \return Client UUID.
   */
  const std::string& getClientUuid ();

  /*! \brief Returns the UUID of the current controlling client.
   *
   * \return Client UUID.
   */
  const std::string& getUuid ();

  /*! \brief Sets the UUID of the master entity in a collaboration mode.
   *
   * \param uuid Master entity UUID.
   */
  void setCollaboratorUuid (const std::string& uuid);

  /*! \brief Gets the UUID of the master entity in a collaboration mode.
   *
   * \return Master entity UUID.
   */
  const std::string& getCollaboratorUuid ();

  /*! \brief Loads the entity basic profile.
   *
   * \param fileUrl Basic profile file URL.
   * \return true if operation is success. false otherwise.
   */
  bool loadBasicPrfl (const std::string& fileUrl);

  /*! \brief Loads the entity basic profile.
   *
   * \param data Pointer to serialized data.
   */
  void getBasisPrflData (std::string *data);

  /*! \brief Loads the entity services profile.
   *
   * \param fileUrl Services profile file URL.
   * \return true if operation is success. false otherwise.
   */
  bool loadServicesProfile (const std::string& fileUrl);

  /*! \brief Packs the entire services profile.
   */
  void compileSvcProfile ();

  /*! \brief Sets the count of the current service profile data record.
   *
   * \param count Service record count.
   */
  void setSvcPrflCount (unsigned int count);

  /*! \brief Gets the count of the current service profile data record.
   *
   * \return Service record count.
   */
  unsigned int getSvcPrflCount ();

  /*! \brief Gets the count of the current service profile data record.
   *
   * \return Service record count.
   */
  unsigned int getCounter ();

  /*! \brief Pushes a service profile data record onto the queue.
   *
   * \svcDataRecord Data record.
   */
  void addSvcDataRecord (const Util::DataRecord& svcDataRecord);

  /*! \brief Retrieves a service profile data from the queue (for transmission).
   *
   * \param svcDataRecord Data record pointer.
   * \return true if a record is retrieved. false if queue is empty.
   */
  bool getSvcDataRecord (Util::DataRecord *svcDataRecord);

  /*! \brief Pushes a incoming message onto the incoming message queue.
   *
   * \param msg Data Message.
   * \param type Message type.
   *
   * \sa getIncomingMsg ()
   */
  void addIncomingMsg (const std::string& msg, unsigned int type);

  /*! \brief Retrieves a message from the incoming queue (to be relayed to the entity).
   *
   * \param msg Message pointer.
   * \param type Message type.
   *
   * \return true if a message is retrieved. false if queue is empty.
   *
   * \sa addIncomingMsg ()
   */
  bool getIncomingMsg (std::string *msg, unsigned int type);

  /*! \brief Pushes a data sequence onto the incoming data sequence queue.
   *
   * \param dataSeq Data sequence.
   * \param type Message type.
   *
   * \sa getIncomingDataSeq ()
   */
  void addIncomingDataSeq (const std::vector <std::string>& dataSeq, unsigned int type);

  /*! \brief Retrieves a data sequence from the incoming data sequence queue
   * (to be relayed to the entity).
   *
   * \param dataSeq Data sequence pointer.
   * \param type Message type.
   * \return true if a data sequence is retrieved. false if queue is empty.
   *
   * \sa addIncomingDataSeq ()
   */
  bool getIncomingDataSeq (std::vector <std::string> *dataSeq, unsigned int type);

  /*! \brief Pushes an incoming data record onto the incoming data record queue.
   *
   * \param dataRec Data record.
   * \param type Message type.
   *
   * \sa getIncomingDataRec ()
   */
  void addIncomingDataRec (const Util::DataRecord& dataRec, unsigned int type);

  /*! \brief Retrieves a data record from the incoming data record queue
   * (to be relayed to the entity).
   *
   * \param dataRec Data sequence pointer.
   * \param type Message type.
   *
   * \return true if a data record is retrieved. false if queue is empty.
   *
   * \sa addIncomingDataRec ()
   */
  bool getIncomingDataRec (Util::DataRecord *dataRec, unsigned int type);

  /*! \brief Pushes a message onto the outgoing message queue.
   *
   * \param msg Message to be sent.
   * \param msgType Message type.
   * \param type Message type (for transmission purposes).
   *
   * \sa getOutgoingMsg ()
   */
  void addOutgoingMsg (const std::string& msg, const std::string& msgType, unsigned int type);

  /*! \brief Retrieves a message from the outgoing message queue (to be transmitted).
   *
   * \param msg Message to be retrieved.
   * \param msgType Message type.
   * \param type Message type (for transmission purposes).
   *
   * \return true if a message was retrieved. false if queue is empty
   *
   * \sa addOutgoingMsg ()
   */
  bool getOutgoingMsg (std::string *msg, std::string *msgType, unsigned int type);

  /*! \brief Pushes a data sequence onto the outgoing data sequence queue.
   *
   * \param msg Data sequence to be sent.
   * \param msgType Message type.
   * \param type Message type (for transmission purposes).
   *
   * \sa getOutgoingDataSeq ()
   */
  void addOutgoingDataSeq (const std::vector <std::string>& dataSeq, const std::string& msgType,
      unsigned int type);

  /*! \brief Retrieves a data sequence from the outgoing data sequence queue (to be transmitted).
   *
   * \param msg Data sequence to be retrieved.
   * \param msgType Message type.
   * \param type Message type (for transmission purposes).
   *
   * \return true if a data sequence was retrieved. false if queue is empty
   *
   * \sa getOutgoingDataSeq ()
   */
  bool getOutgoingDataSeq (std::vector <std::string> *dataSeq, std::string *msgType,
      unsigned int type);

  /*! \brief Pushes a data record onto the outgoing data record queue.
   *
   * \param msg Data record to be sent.
   * \param msgType Message type.
   * \param type Message type (for transmission purposes).
   *
   * \sa getOutgoingDataRec ()
   */
  void addOutgoingDataRec (const Util::DataRecord& dataRec, const std::string& msgType,
      unsigned int type);

  /*! \brief Retrieves a data record from the outgoing data record queue (to be transmitted).
   *
   * \param msg Data record to be retrieved.
   * \param msgType Message type.
   * \param type Message type (for transmission purposes).
   *
   * \return true if a data record was retrieved. false if queue is empty
   *
   * \sa addOutgoingDataRec ()
   */
  bool getOutgoingDataRec (Util::DataRecord *dataRec, std::string *msgType, unsigned int type);

  /*! \brief Pushes an entity message onto the outgoing entity message queue.
   *
   * \param msg Data record to be sent.
   * \param msgType Message type.
   * \param type Message type (for transmission purposes).
   *
   * \sa getOutgoingEntityMsg ()
   */
  void addOutgoingEntityMsg (const std::string& msg, const std::string& msgType,
      const std::string& destId);

  /*! \brief Retrieves an entity message from the outgoing entity message queue (to be transmitted).
   *
   * \param msg Entity message to be retrieved.
   * \param msgType Message type.
   * \param destId Destination entity UUID (for transmission purposes).
   *
   * \return true if an entity message was retrieved. false if queue is empty
   *
   * \sa addOutgoingEntityMsg ()
   */
  bool getOutgoingEntityMsg (std::string *msg, std::string *msgType, std::string *destId);

  /*! \brief Pushes an entity data sequence onto the outgoing entity data sequence queue.
   *
   * \param msg Data sequence to be sent.
   * \param msgType Message type.
   * \param destId Destination entity UUID (for transmission purposes).
   *
   * \sa getOutgoingEntityDataSeq ()
   */
  void addOutgoingEntityDataSeq (const std::vector <std::string>& dataSeq,
      const std::string& msgType, const std::string& destId);

  /*! \brief Retrieves an entity data sequence from the outgoing entity data sequence queue
   * (to be transmitted).
   *
   * \param msg Entity data sequence to be retrieved.
   * \param msgType Message type.
   * \param destId Destination entity UUID (for transmission purposes)
   *
   * \return true if an entity data sequence was retrieved. false if queue is empty.
   *
   * \sa addOutgoingEntityDataSeq ()
   */
  bool getOutgoingEntityDataSeq (std::vector <std::string> *dataSeq, std::string *msgType,
      std::string *destId);

  /*! \brief Pushes a new task onto the task queue.
   *
   * \param taskSeq Task data.
   * \param type Task type.
   *
   * \sa getPendingTask ()
   */
  void addNewTask (std::vector <std::string>& taskSeq, unsigned int type);

  /*! \brief Retrieves a task from the task queue (to be relayed to the entity).
   *
   * \param taskSeq Task data.
   * \param type Task type.
   *
   * \return true if a task was retrieved. false if queue is empty.
   *
   * \sa addNewTask ()
   */
  bool getPendingTask (std::vector <std::string> *taskSeq, unsigned int type);

  /*! \brief Pushes a collaborative task onto the collaborative task queue.
   *
   * \param dataRec Task data.
   * \param type Task type.
   *
   * \sa getPendingCollaborativeTask ()
   */
  void addNewCollaborativeTask (Util::DataRecord& dataRec, unsigned int type);

  /*! \brief Retrieves a collaborative from the collaborative task queue.
   *
   * \param dataRec Task data.
   * \param type Task type.
   *
   * \return true if a task was retrieved. false if queue is empty.
   *
   * \sa addNewCollaborativeTask ()
   */
  bool getPendingCollaborativeTask (Util::DataRecord *dataRec, unsigned int type);

  /*! \brief Sends a message to the robotic application and retrieves a reply.
   *
   * \param prefix Message prefix.
   * \param body Message body.
   * \param reply Message reply.
   *
   * \return true if sending is success. false otherwise.
   *
   * \sa sendEntitySyncMsg ()
   */
  bool sendEntitySyncMsg (const std::string& prefix, const std::string& body, std::string *reply);

  /*! \brief Sends a message to the robotic application and retrieves a reply.
   *
   * \param prefix Message prefix.
   * \param body Message body.
   * \param reply Message reply.
   *
   * \return true if sending is success. false otherwise.
   *
   * \sa sendEntitySyncMsg
   */
  bool sendEntitySyncMsg (const std::string& prefix, const std::string& body, char *reply,
      unsigned int size);

  /*! \brief Signals if the entity manager is in an error state.
   *
   * \param flag Error state flag.
   * \param body Message body.
   * \param reply Message reply.
   *
   * \sa getErrState ()
   */
  void setErrState (const bool& flag);

  /*! \brief Signals if the entity manager is in an error state.
   *
   * \param flag Error state flag.
   * \param body Message body.
   * \param reply Message reply.
   *
   * \sa setErrState ()
   */
  bool getErrState ();

  int dispatchToEntity (std::string& data, std::string *reply, bool *taskFlag);

  /*! \brief Resets all parameters and configurations after a recent release.
   */
  void releaseReset ();

  /*! \brief Resets all parameters, configurations and connections after lossing connection with
   * the entity application.
   */
  void resetAll ();

  /*! \brief Clears all pending client tasks.
   */
  void clearPendingClientTasks ();

  /*! \brief Clears all pending collaborative tasks (received from controlling client).
   */
  void clearPendingClientCollaborativeTasks ();

  /*! \brief Clears all pending collaborative tasks (received from a collaborative
   * entity).
   */
  void clearPendingEntityTasks ();

  /*! \brief Clears all queues and stored data.
   */
  void clearAllData ();

  /*! \brief Closes connection with entity.
   */
  void closeEntityConn ();

  /*! \brief Peforms cleanup before shutdown.
   */
  void close ();

  /*! \brief This function is used by the different states to log internal events and
   * state transitions.
   *
   * \param state State name.
   * \param type Message type.
   * \param msg Message content.
   */
  void stateLog (const std::string& state, const std::string& type, const std::string& msg);

protected:
  friend class State;
  State *currState;                         /*!< Current state. */

  const std::string PORTS_CFG;             /*!< Ports configurations file URL. */
  const std::string GENERAL_CFG;           /*!< General configurations file URL. */
  const unsigned int PROFILE_CFG_COUNT;    /*!< Expected profile configurations parameter count. */
  const unsigned int GENERAL_CFG_COUNT;    /*!< Expected general configurations parameter count. */
  unsigned int FIELD_NAME_CC;              /*!< Never exceed length for 'entity name' parameter. */
  unsigned int FIELD_TYPE_CC;              /*!< Never exceed length for 'entity type' parameter. */
  unsigned int FIELD_DESCR_CC;             /*!< Never exceed length for 'entity descr' parameter. */

  std::string entityUuid;               /*!< Entity UUID. */
  std::string clientUuid;               /*!< Controlling client UUID. */
  std::string collaboratorEntityUuid;   /*!< Master entity UUID (in collaboration mode). */
  Data::BasicProfile basicPrfl;         /*!< Basic profile. */
  std::string basicPrflData;            /*!< Basic profile data. */
  unsigned int svcPrflCount;            /*!< Total number of service records. */
  unsigned int counter;                 /*!< Count of a transmitted service record. */
  bool entityOnline;    /*!< Flag indicating if the manager is connected to the robotic app. */
  bool connected;       /*!< Flag indicating if the manager is connected to the server. */
  bool connectSignal;   /*!< Flag indicating if establishing connection to the server is granted. */
  bool checkedOut;      /*!< Flag indicating if entity is checked-out. */
  bool requested;       /*!< Flag indicating if there is a pending client check-out request. */
  bool released;        /*!< Flag indicating if entity has been recently released. */
  bool collaboration;   /*!< Flag indicating if entity is the master in collaboration mode. */
  bool slave;           /*!< Flag indicating if entity is a slave in collaboration mode. */
  bool stop;            /*!< Flag indicating if there is a pending stop command. */
  std::string stopMsg;  /*!< Informative message accompanying a stop command. */
  bool errState;        /*!< Flag indicating if entity is in an error state. */
  ///FIXME Temporary fix
  bool initiator;   /*!< Flag indicating if the entity is the initiator of a collaboration mode. */

  IceUtil::Mutex connMutex0;    /*!< Mutex for accessing shared variable 'entityOnline'. */
  IceUtil::Mutex connMutex1;    /*!< Mutex for accessing shared variable 'connected' & 'connectSignal'. */
  IceUtil::Mutex dataMutex;     /*!< Mutex for accessing shared variables and queues. */
  const Ice::LoggerPtr iceLogger_Ptr; /*!< Ice logging utility */
  const bool logFlag;                 /*!< bool flag to toggle logging */

  /*!< Incoming and outgoing data and messages queues. */
  std::queue <Util::DataRecord> svcRecordQueue;
  std::queue <std::string> incomingClientMsgQueue;
  std::queue <std::vector <std::string> > incomingClientDataSeqQueue;
  std::queue <Util::DataRecord> incomingClientDataRecQueue;

  std::queue <std::string> incomingEnvironmentMsgQueue;
  std::queue <std::string> incomingEntityMsgQueue;

  std::queue <std::string> outgoingClientMsgQueue;
  std::queue <std::string> outgoingClientMsg_typeQueue;
  std::queue <std::vector <std::string> > outgoingClientDataSeqQueue;
  std::queue <std::string> outgoingClientDataSeq_typeQueue;
  std::queue <Util::DataRecord> outgoingClientDataRecQueue;
  std::queue <std::string> outgoingClientDataRec_typeQueue;

  std::queue <std::string> outgoingEnvironmentMsgQueue;
  std::queue <std::string> outgoingEnvironmentMsg_typeQueue;
  std::queue <std::vector <std::string> > outgoingEnvironmentDataSeqQueue;
  std::queue <std::string> outgoingEnvironmentDataSeq_typeQueue;

  std::queue <std::string> outgoingEntityMsgQueue;
  std::queue <std::string> outgoingEntityMsg_typeQueue;
  std::queue <std::string> outgoingEntityMsg_destQueue;
  std::queue <std::vector <std::string> > outgoingEntityDataSeqQueue;
  std::queue <std::string> outgoingEntityDataSeq_typeQueue;
  std::queue <std::string> outgoingEntityDataSeq_destQueue;

  std::queue <std::vector <std::string> > pendingClientTaskQueue;
  std::queue <std::vector <std::string> > pendingEntityTaskQueue;
  std::queue <Util::DataRecord> pendingClientCollaborativeTaskQueue;

  void switchState (State *newState);
};

#endif /* ENTITYMANAGER_H_ */
