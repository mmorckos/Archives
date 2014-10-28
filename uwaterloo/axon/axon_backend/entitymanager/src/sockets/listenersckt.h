/*
 * listenersckt.h
 *
 *  Created on: 2011-11-07
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef LISTENERSCKT_H_
#define LISTENERSCKT_H_

#include "socket.h"

class ListenerSckt : public Socket
{

public:
  /*! \brief Constructor
   */
  ListenerSckt ();

  /*! \brief Initializes a new connection.
   *
   * Sets up a new socket and binds it to the given address information.
   *
   * \param flag whether it is a UPD or TCP based connection.
   * \param recvTimeout indicates the timeout for receiving new messages.
   * \param sendTimeout indicates the timeout for sending new messages.
   * \returns number of the port bound to. A port number of 0 or -1 indicates a failure.
   * \sa closeConn ()
   */
  bool init (bool proto = SCKTTCP, long recvTimeout = 1, long sendTimeout = 1);

  /*! \brief Finalizes connection issues.
   *
   *  Sets up timeout options. In case of TCP connection it is responsible for listening and
   *  accepting new connection.
   *
   * \returns true if operation was success. false otherwise.
   * \sa closeConn ()
   */
  int openConn ();

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

#endif /* LISTENERSCKT_H_ */
