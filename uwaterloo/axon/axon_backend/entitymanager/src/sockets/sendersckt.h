/*
 * sendersckt.h
 *
 *  Created on: 2011-11-07
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef SENDERSCKT_H_
#define SENDERSCKT_H_

#if defined (__linux__)
#include <netdb.h>
#endif

#include "socket.h"

class SenderSckt : public Socket
{

public:
  /*! \brief Constructor
   */
  SenderSckt ();

  /*! \brief Initializes a new connection.
   *
   * Sets up a new socket and connects to another listener entity.
   *
   * \param flag whether it is a UPD or TCP based connection.
   * \param recvTimeout indicates the timeout for receiving new messages.
   * \param sendTimeout indicates the timeout for sending new messages.
   * \returns number of the port bound to. A port number of 0 or -1 indicates a failure.
   * \sa closeConn ()
   */
  bool init (bool proto = SCKTTCP, long recvTimeout = 1, long sendTimeout = 1);

  /*! \brief Finalizes connection issues by setting timeout options.
   *
   * \returns true if operation was success. false otherwise.
   * \sa closeConn ()
   */
  int openConn ();

  /*! \brief Acquires the IP address of the entity on the other end of the connection.
   *
   * \param ipAddr the IP address of type string.
   * \returns true if the IP address is valid. false otherwise.
   * \sa getIpAddr ()
   */
  bool setIpAddr (std::string ipAddr);

  /*! \brief Returns the assigned IP address.
   *
   * \returns IP address of type string.
   * \sa setIpAddr (std::string ipAddr)
   */
  std::string getIpAddr ();

  /*! \brief Sends a message to the entity on the other end of the link.
   *
   * \param msg the message to be sent of type string.
   * \returns an integer indicating the operation status.
   * \sa getMsg ()
   */
  int sendMsg (const std::string& msg);
  int sendMsg (const char *msg, const unsigned int size);

  /*! \brief Receives a message from the entity on the other end of the link.
   *
   * \returns msg the received message of type string, or an empty string if nothing was received.
   * \sa sendMsg (std::string msg)
   */
  int getMsg (std::string *msg);
  int getMsg (char *msg, const unsigned int size);

protected:
  int sendMsg_Helper (const char *msg, const unsigned int size);
};

#endif /* SENDERSCKT_H_ */
