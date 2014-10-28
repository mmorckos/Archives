/*
 * entityinterfacer.h
 *
 *  Created on: 2012-01-30
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef ENTITYINTERFACER_H_
#define ENTITYINTERFACER_H_

#include <iostream>
#include <string>
#include <queue>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "communicator.h"
#include "sockets/listenersckt.h"
#include "common/global.h"
#include "util/logger.h"

const unsigned int CONFIG_PARAM_COUNT = 5;
const unsigned int READINESS_0 = 256;
const unsigned int MAXREADINESS = 1023;
const std::string PORTSFILE = "../config/ports.cfg";

class EntityInterfacer
{

public:
  /*! \brief Constructor.
   */
  EntityInterfacer ();

  /*! \brief Destructor.
   */
  virtual ~EntityInterfacer ();

  /*! \brief Initializes the entity interface.
   *
   * \param fileName Ports configurations file name of type string.
   * \param logFlag Logging flag.
   */
  bool init (const std::string& fileName = PORTSFILE, const bool logFlag = true);

  /*! \brief Starts the entity interface. The interfacer will then establish connection with the
   * entity manager.
   *
   * \return true if start-up is success. false otherwise.
   */
  bool start ();

  /*! \brief Stops the entity interface.
   */
  void stop ();

  /*! \brief API function used to upload the basic profile.
   *
   * \param fileUrl File location of type string.
   */
  void uploadBasicProfile (const std::string& fileUrl);

  /*! \brief API function used to upload the services profile.
   *
   * \param fileUrl File location of type string.
   */
  void uploadServiceProfile (const std::string& fileUrl);

  /*! \brief API function used to set the current status of the entity, either busy or available.
   *
   * \param flag The entity status.
   */
  void setStatus (const bool& flag);

  /*! \brief API function used to update the current location of the entity in the environment.
   *
   * \param xPos The entity X position.
   * \param yPos The entity Y position.
   * \param zPos The entity Z position.
   *
   * \return true if data is valid. false otherwise.
   */
  bool updatePosition (const double& xPos, const double& yPos, const double& zPos = 0);

  /// TODO
  /*! \brief API function used to send a data file to the client.
   *
   * \param healthData Entity health data of type string.
   */
  void updateHealth (const std::string& healthData);

  /*! \brief API function used to send a simple informative message to the
   * client.
   *
   * \param msgBody Message body of type string.
   */
  void sendInfoMsg (const std::string& msgBody);

  /*! \brief API function used to send the feedback of a recently finished tasked.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param taskStatus Task status, either success or failure.
   * \param fdbkMsg Informative feedback message of type string.
   * \return true if sending feedback is success. false otherwise.
   */
  bool sendTaskFeedback (const unsigned int& serviceId, const bool& taskStatus,
      const std::string& fdbkMsg = "");

  /// TODO
  /*! \brief API function used to send a data file to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param dataUrl File location of type string.
   */
  bool sendData (const std::string& serviceId, const std::string& dataUrl);

  /*! \brief API function used to send an integer parameter to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param value Parameter value of type.
   * \return true if operation is success. false otherwise.
   */
  bool sendIntParam (const unsigned int& serviceId, const unsigned int& id, const int& value);

  /*! \brief API function used to send an double parameter to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param value Parameter value of type double.
   * \return true if operation is success. false otherwise.
   */
  bool sendDoubleParam (const unsigned int& serviceId, const unsigned int& id, const double& value);

  /*! \brief API function used to send an string parameter to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param value Parameter value of type string.
   * \return true if operation is success. false otherwise.
   */
  bool sendStringParam (const unsigned int& serviceId, const unsigned int& id,
      const std::string& value);

  /*! \brief API function used to send an bool parameter to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param value Parameter value of type bool.
   * \return true if operation is success. false otherwise.
   */
  bool sendBooleanParam (const unsigned int& serviceId, const unsigned int& id, const bool& value);

  /// TODO
  /*! \brief API function used to send an array of integers to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param list Parameter value of type vector <int>.
   * \return true if operation is success. false otherwise.
   */
  bool sendIntListParam (const unsigned int& serviceId, const unsigned int& id,
      const std::vector <int>& list);

  /// TODO
  /*! \brief API function used to send an array of doubles to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param list Parameter value of type vector <double>.
   * \return true if operation is success. false otherwise.
   */
  bool sendDoubleListParam (const unsigned int& serviceId, const unsigned int& id,
        const std::vector <double>& list);

  /// TODO
  /*! \brief API function used to send an array of strings to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param list Parameter value of type vector <string>.
   * \return true if operation is success. false otherwise.
   */
  bool sendStringListParam (const unsigned int& serviceId, const unsigned int& id,
        const std::vector <std::string>& list);

  /// TODO
  /*! \brief API function used to send an array of strings to the client.
   *
   * \param serviceId Local service ID of type unsigned int.
   * \param id Parameter ID of type int.
   * \param list Parameter value of type vector <string>.
   * \return true if operation is success. false otherwise.
   */
  bool sendBoolListParam (const unsigned int& serviceId, const unsigned int& id,
        const std::vector <bool>& list);

  /*! \brief API function used to set the object in the entity application that will be
   * interacting with the entity interfacer.
   *
   * \param c The callee object pointer of type void (Generic).
   * \return true if operation is success. false otherwise.
   */
  bool setCallee (void *c);

  /*! \brief API function used to set the callback for receiving a new task.
   *
   * \param ptrFunc Function pointer. The target function should accept two parameters: the callee
   * of type void*, and the service ID of type unsigned int.
   */
  void setTaskCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId));

  /*! \brief API function used to set the callback for receiving an integer parameter.
   *
   * \param ptrFunc Function pointer. The target function should accept four parameters: the callee
   * of type void*, the service ID of type unsigned int, the parameter ID of type int, and the
   * parameter value of type int.
   */
  void setIntParamCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const int& value));

  /// TODO
  void setIntParamListCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::vector <int>& values));

  /*! \brief API function used to set the callback for receiving an double parameter.
   *
   * \param ptrFunc Function pointer. The target function should accept four parameters: the callee
   * of type void*, the service ID of type unsigned int, the parameter ID of type int, and the
   * parameter value of type double.
   */
  void setDoubleParamCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const double& value));

  /// TODO
  void setDoubleParamListCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::vector <double>& values));

  /*! \brief API function used to set the callback for receiving an string parameter.
   *
   * \param ptrFunc Function pointer. The target function should accept four parameters: the callee
   * of type void*, the service ID of type unsigned int, the parameter ID of type int, and the
   * parameter value of type string.
   */
  void setStringParamCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::string& value));

  /// TODO
  void setStringParamListCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::vector <std::string>& values));

  /*! \brief API function used to set the callback for receiving an bool parameter.
   *
   * \param ptrFunc Function pointer. The target function should accept four parameters: the callee
   * of type void*, the service ID of type unsigned int, the parameter ID of type int, and the
   * parameter value of type bool.
   */
  void setBooleanParamCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const bool& value));

  /// TODO
  void setBooleanParamListCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::vector <bool>& values));

  /*! \brief API function used to set the callback for receiving a resource.
   *
   * \param ptrFunc Function pointer. The target function should accept four parameters: the callee
   * of type void*, the service ID of type unsigned int, the resource ID of type int, and the
   * resource file URL of type string.
   */
  void setResourceCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
      const unsigned int& resId, const std::string& url));

  void setUpdateCallback (bool (*ptrFunc) (void *callee, const std::string& msg));

  /*! \brief API function used to set the callback for receiving stop signals.
   *
   * \param ptrFunc Function pointer. The target function should accept two parameters: the callee
   * of type void*, and simple informative message of type string.
   */
  void setStopCallback (bool (*ptrFunc) (void *callee, const std::string& msg));

  /*! \brief Used to test that all callbacks are set.
   */
  void testCallbacks ();

protected:
  friend class Communicator;

  std::string status;               /*!< Current entity status. */
  bool regsitered;                  /*!< Registered flag. */
  std::string basicPrflUrl;         /*!< Basic profile file url. */
  std::string svcPrflUrl;           /*!< Services profile file url. */
  ListenerSckt commSckt;            /*!< Main communication socket with the entity manager. */
  CommunicatorPtr communicator_Ptr; /*!< Communicator pointer. */
  std::queue <std::string> dataQueue; /*!< Data queue. */
  std::queue <std::string> infoMsgQueue; /*!< Informative messages queue. */
  std::vector <double> currPos;          /*!< Current location data. */
  char posData [BUFFSIZE];               /*!< Location data. */
  IceUtil::TimerPtr timer_Ptr;           /*!< ICE Timer pointer for thread scheduling. */
  IceUtil::Mutex startMutex;             /*!< Shared data and resources ICE mutex. */
  IceUtil::Mutex stopMutex;              /*!< Shared data and resources ICE mutex. */
  IceUtil::Mutex dataMutex;              /*!< Shared data and resources ICE mutex. */
  IceUtil::Mutex sendMutex;              /*!< Shared data and resources ICE mutex. */
  IceUtil::Mutex emergencyStopMutex;     /*!< Shared data and resources ICE mutex. */
  IceUtil::Mutex logMutex;               /*!< Shared data and resources ICE mutex. */
  Util::Logger logger;                   /*!< Logger. */
  bool logFlag;                          /*!< Log flag. */
  unsigned int readiness;                /*!< Entity interfacer readiness. Signals if all callbacks and configurations are set.*/
  bool connected;                        /*!< Entity interfacer readiness. */
  bool running;                          /*!< Running flag. */
  int execTaskStatus;                    /*!< Status of recently finished task. */
  std::string execTaskFdbk;              /*!< Entity interfacer readiness. */

  void *callee;
  bool (*taskCallback_Ptr) (void *callee, const unsigned int& serviceId);
  bool (*intParamCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const int& value);

  bool (*intParamListCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::vector <int>& values);

  bool (*doubleParamCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const double& value);

  bool (*doubleParamListCallback_Ptr) (void *callee, const unsigned int& serviceId,
        const unsigned int& id, const std::vector <double>& values);

  bool (*stringParamCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::string& value);

  bool (*stringParamListCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::vector <std::string>& values);

  bool (*booleanParamCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const bool& value);

  bool (*booleanParamListCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& id, const std::vector <bool>& values);

  bool (*resourceCallback_Ptr) (void *callee, const unsigned int& serviceId,
      const unsigned int& resId, const std::string& url);

  bool (*updateCallback_Ptr) (void *callee, const std::string& msg);

  bool (*stopCallback_Ptr) (void *callee, const std::string& msg);

  /*! \brief Establishes connection with entity manager.
   *
   * \return true if connection is success. false otherwise.
   */
  bool connect ();

  /*! \brief Closes connection with entity manager.
   */
  void disconnect ();

  /*! \brief Returns the current status of the entity.
   *
   * \return Entity status of type string.
   */
  std::string getStatus ();

  /*! \brief Sets the registration status of the entity, either registered to a client or idle.
   *
   * \param flag Registration flag of type bool.
   */
  void setRegistered (const bool& flag);

  /*! \brief Returns the registration status of the entity.
   *
   * \return flag Registration flag of type bool.
   */
  bool getRegistered ();

  /*! \brief Stores the basic profile file url in a message, to be sent to the entity manager.
   *
   * \param msg Pointer to the message of type string.
   * \return true if file url is valid. false otherwise.
   */
  bool getBasicProfile (std::string *msg);

  /*! \brief Stores the services profile file url in a message, to be sent to the entity manager.
   *
   * \param msg Pointer to the message of type string.
   * \return true if file url is valid. false otherwise.
   */
  bool getSvcProfile (std::string *msg);

  /*! \brief Stores the current location of the entity in a message, to be sent to the entity
   * manager.
   *
   * \param msg Pointer to the message of type char.
   * \param size Message size of type size_t.
   * \return true if data is valid. false otherwise.
   */
  bool getPositionUpdate (char *data, size_t size);

  /*! \brief Addes a data item onto ongoing queue, to be sent to the entity manager.
   *
   * \param msg Reference to the data item of type string.
   */
  void addDataItem (const std::string& msg);

  /*! \brief Retrieves a data item from ongoing queue, to be sent to the entity manager.
   *
   * \param msg Pointer to the data item of type string.
   * \return true if the queue has a message. false if queue is empty.
   */
  bool getDataItem (std::string *msg);

  /*! \brief Addes an informative message onto ongoing queue, to be sent to the entity manager.
   *
   * \param msg Reference to the messageof type string.
   */
  void addInfoMsg (const std::string& msg);

  /*! \brief Retrieves an informative message from ongoing queue, to be sent to the entity manager.
   *
   * \param msg Pointer to the message of type string.
   * \return true if the queue has a message. false if queue is empty.
   */
  bool getInfoMsg (std::string *msg);

  /*! \brief Retrieves the feedback of a recently finished task, to be sent to the entity manager.
   *
   * \param status Pointer to task status of type int.
   * \param fdbk Pointer to the feedback message of type string.
   */
  void getTaskFeedback (int *status, std::string *fdbk);

  /*! \brief Packs a message, to be sent to the entity manager.
   *
   * \param prefix Reference to the message prefix of type string.
   * \param msgBody Reference to the message body of type string.
   * \param msg Pointer to message of type string.
   */
  void compileMsg (const std::string& prefix, const std::string& msgBody, std::string *msg);

  void emergencyStop (const std::string& statusMsg = "");

  /*! \brief Logs events and status of the entity interfacer.
   *
   * \param type Log event type of type string.
   * \param type Log message of type string.
   */
  void log (const std::string& type, const std::string& msg);
};

#endif /* ENTITYINTERFACER_H_ */
