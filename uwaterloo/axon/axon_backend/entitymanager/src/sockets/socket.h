/*
 * socket.h
 *
 *  Created on: 2011-11-07
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#if defined (__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#elif defined (_WIN32)
#include <winsock.h>
#include <winerror.h>
#endif
#include <iostream>

#if defined (_WIN32)
#pragma comment (lib, "wsock32.lib")
#endif

#if defined (__linux__)
#define closesocket(s) close(s);
#endif

const bool SCKTUDP = false;
const bool SCKTTCP = true;
const unsigned int MAXCONN = 1;
const unsigned int MINPORT = 5500;
const unsigned int MAXPORT = 8000;

class Socket
{

public:
  /*! \brief Constructor
   */
  Socket ():
    ipAddr ("127.0.0.1"),
    portNum0 (0),
    portNum1 (0),
    portNum (0),
    status (2)
  {}

  /*! \brief Destructor
   */
  virtual ~Socket ()
  {}

  /*! \brief Sets a port range for binding.
   *
   * \param portNum0 starting port number.
   * \param portNum1 ending port number.
   * \return true if port range is valid. false otherwise.
   */
  bool setPortRange (unsigned int portNum0, unsigned int portNum1)
  {
    if (portNum0 < MINPORT || portNum1 > MAXPORT || portNum0 > portNum1)
      {
        setInternalStatus ("ERROR! Invalid port numbers range.");
        return false;
      }

    this->portNum0 = portNum0;
    this->portNum1 = portNum1;

    return true;
  }

  /*! \brief Returns the port number
   *
   * \return port number of type unsigned short.
   * \sa setPort (unsigned int portNum)
   */
  unsigned short getPort ()
  {
    return portNum;
  }

  /*! \brief Returns socket's connection status
   *
   * \return status of type integer.
   */
  int getStatusCode ()
  {
    return status;
  }

  /*! \brief Sets the current connection status message
   *
   * \return status message of type string.
   * \sa setStatMsg (const std::string msg)
   */
  std::string getInternalStatus ()
  {
    return internalStatus;
  }

  /*! \brief Closes connection and does clean-up.
   *
   * \sa openConn (bool flag)
   */
  void closeConn ()
  {
#if defined (_WIN32)
    WSACleanup ();
#endif
    closesocket (scktFd);
  }

  virtual bool init (bool proto = SCKTTCP, long recvTimeout = 1, long sendTimeout = 1) = 0;
  virtual int openConn () = 0;
  virtual int sendMsg (const std::string& msg) = 0;
  virtual int sendMsg (const char *msg, const unsigned int size) = 0;
  virtual int getMsg (std::string *msg) = 0;
  virtual int getMsg (char *msg, const unsigned int size) = 0;

protected:
  struct sockaddr_in srcAddr;
  struct sockaddr_in destAddr;
  struct hostent *destHost;
  struct timeval recvTimeout;
  struct timeval sendTimeout;
#if defined (__linux__)
  socklen_t addrLen;
#elif defined (_WIN32)
  int addrLen;
#endif
  int scktFd;                     /*!< Socket descriptor */
  std::string ipAddr;             /*!< IP address */
  unsigned short portNum0;        /*!< First port number */
  unsigned short portNum1;        /*!< Last port number */
  unsigned short portNum;         /*!< Used port number */
  unsigned short port;            /*!< Port number in network order (in big endian notation) */
  bool scktType;                  /*!< Socket type (TCP or UDP) */
  int status;                     /*!< Connection status code */
  std::string internalStatus;     /*!< Status message (for logging purposes) */

#if defined (_WIN32)
  WSADATA wsaData;
#endif

  /*! \brief Sets the current connection status message
   *
   * For internal use.
   *
   * \param msg the status string.
   * \sa getStatMsg ()
   */
  void setInternalStatus (const std::string msg)
  {
    internalStatus = msg;
    internalStatus.append (" Details: ");
#if defined (__linux__)
    internalStatus.append (strerror (errno));
#elif defined (_WIN32)
    char buff [32];

    strerror_s (buff, 32, errno);
    internalStatus.append (buff);
#endif
  }

  virtual int sendMsg_Helper (const char *msg, const unsigned int size) = 0;
};

#endif /* SOCKET_H_ */
