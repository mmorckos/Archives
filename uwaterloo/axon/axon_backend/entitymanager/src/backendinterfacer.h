/*
 * backendinterfacer.h
 *
 *  Created on: 2012-01-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef BACKENDINTERFACER_H_
#define BACKENDINTERFACER_H_

#include <Ice/Ice.h>
#if defined (__linux__)
#include "./sockets/sendersckt.h"
#elif defined (__WIN32)
#include "..\\comm\\sendersckt.h"
#endif

const unsigned int CFG_PARAM_COUNT = 6;           /*!< Minimum number of configuration parameter. */
const std::string CFG_PORT_PATH = "../config/ports.cfg"; /*!< Ports configurations file URL. */

class BackEndInterfacer
{

public:
  /*! \brief Constructor.
   */
  BackEndInterfacer (const Ice::LoggerPtr& logger, const bool& flag);

  /*! \brief Destructor.
   */
  virtual ~BackEndInterfacer ();

  /*! \brief Initialization function.
   *
   * Parses and loads initial configurations
   *
   * \param fileName Configurations file.
   *
   * \return true if initialization is success. false otherwise.
   */
  bool init (const std::string& fileName = CFG_PORT_PATH);

  /*! \brief Connection function.
   *
   * Starts the actual connection with the entity and handles simple identity exchange.
   *
   * \return true if initialization is success. false otherwise.
   *
   * \sa closeConn ()
   */
  bool openConn ();

  /*! \brief Closes the backend interfacer. Shuts down the heart-beat and messages socket.
   *
   * \sa openConn ()
   */
  void closeConn ();

  /*! \brief Sends a string message to the connected entity.
   *
   * \param msg The message to send.
   * \param reply The reply message.
   *
   * \return true if sending is success. false otherwise.
   */
  bool sendSyncMsg (const std::string& msg, std::string *reply);

  /*! \brief Sends a char array message to the connected entity.
   *
   * \param msg The message to send.
   * \param reply The reply message.
   * \param reply Size of the reply message.
   *
   * \return true if sending is success. false otherwise.
   */
  bool sendSyncMsg (const std::string& msg, char *reply, unsigned int size);

  /*! \brief Sends a char array message to the connected entity.
   *
   * \param msg The message to send.
   * \param size0 Size of the message.
   * \param reply The reply message.
   * \param reply Size of the reply message.
   *
   * \return true if sending is success. false otherwise.
   */
  bool sendSyncMsg (const char *msg, unsigned int size0, char *reply, unsigned int size1);

protected:
  SenderSckt commSckt;                /*!< Main communications socket */
  std::string internalStatus;         /*!< Status message (for logging purposes) */
  const Ice::LoggerPtr iceLogger_Ptr; /*!< Ice logging utility */
  const bool logFlag;                 /*!< bool flag to toggle logging */
  IceUtil::Mutex dataMutex;
};

#endif /* BACKENDINTERFACER_H_ */
