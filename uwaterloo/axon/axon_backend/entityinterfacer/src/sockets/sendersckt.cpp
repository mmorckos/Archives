/*
 * sendersckt.cpp
 *
 *  Created on: 2011-11-07
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "sendersckt.h"

#if defined (__linux__)
#include "../common/global.h"
#include "../common/status.h"
#elif defined (_WIN32)
#include "..\\common\\common.h"
#endif

SenderSckt::SenderSckt () :
  Socket ()
{}

bool
SenderSckt::init (bool proto, long recvTimeout, long sendTimeout)
{
  unsigned short port;

  if (recvTimeout <= 0 || sendTimeout <= 0)
    {
      setInternalStatus ("ERROR! Invalid values for receive timeout or/and send timeout");
      return false;
    }

  scktType = proto;
  this->recvTimeout.tv_sec = recvTimeout;
  this->recvTimeout.tv_usec = 0;
  this->sendTimeout.tv_sec = sendTimeout;
  this->sendTimeout.tv_usec = 0;

  return true;
}

int
SenderSckt::openConn ()
{
  unsigned short tmp;

  tmp = portNum0;
  port = (unsigned short) htons (portNum0);
  addrLen = sizeof (struct sockaddr_in);
  destAddr.sin_family = AF_INET;
  destAddr.sin_addr = *((struct in_addr *) destHost->h_addr);
  destAddr.sin_port = port;

  if ((scktFd = socket (AF_INET, scktType ? SOCK_STREAM : SOCK_DGRAM, 0)) == ERR)
    {
      setInternalStatus ("ERROR! Failed to create socket.");
      return false;
    }

  if (scktType == SCKTTCP)
    {
      while (tmp <= portNum1 && connect (scktFd, (struct sockaddr *) &destAddr,
          sizeof (destAddr)) == ERR)
        {
          port = (unsigned short) htons (++tmp);
          destAddr.sin_port = port;
         }
    }

  if (tmp > portNum1)
    {
      setInternalStatus ("ERROR! Failed to establish connection.");
      return ERR;
    }

  if (setsockopt (scktFd, SOL_SOCKET, SO_RCVTIMEO, (char *) &recvTimeout, sizeof (recvTimeout)) < 0)
    {
      setInternalStatus ("ERROR! Failed to set receiving timeout.");
      return ERR;
    }

  if (setsockopt (scktFd, SOL_SOCKET, SO_SNDTIMEO, (char *) &sendTimeout, sizeof (sendTimeout)) < 0)
    {
      setInternalStatus ("ERROR! Failed to set sending timeout.");
      return ERR;
    }

  return tmp;
}

bool
SenderSckt::setIpAddr (std::string ipAddr)
{
#if defined (_WIN32)
  if (WSAStartup (MAKEWORD (1,1), &wsaData) != 0)
      return false;
#endif
  destHost = (struct hostent *) gethostbyname (ipAddr.c_str ());

  if (destHost == NULL)
    {
      setInternalStatus ("ERROR! Bad IP address.");
      return false;
    }

  this->ipAddr.append (ipAddr.c_str ());

  return true;
}

std::string
SenderSckt::getIpAddr ()
{
  return ipAddr;
}

int
SenderSckt::sendMsg (const std::string& msg)
{
  return sendMsg_Helper (msg.c_str (), msg.size ());
}

int
SenderSckt::sendMsg (const char *msg, unsigned int size)
{
  return sendMsg_Helper (msg, size);
}

int
SenderSckt::getMsg (std::string *msg)
{
  char buff [BUFFSIZE];

  if (scktType == SCKTUDP)
    {
      status = recvfrom (scktFd, buff, BUFFSIZE, 0, (struct sockaddr *) &destAddr, &addrLen);
    }
  else
    {
#if defined (__linux__)
      status = read (scktFd, buff, BUFFSIZE);
#elif defined (_WIN32)
      status = recv (scktFd, buff, BUFFSIZE, 0);
#endif
    }

  if (status > 0)
    {
      buff [status] = '\0';
      (*msg) = "";
      msg->append (buff);
      status = OK;
    }
  else if (status == ERR)
    {
      if (errno == EAGAIN)
        {
          setInternalStatus ("Receiving timed out.");
          status = TIMEOUT;
        }
      else
        {
          setInternalStatus ("ERROR! No message received.");
        }
    }
  else if (status == CLOSED)
    {
      setInternalStatus ("Connection dropped.");
    }

  return status;
}

int
SenderSckt::getMsg (char *msg, const unsigned int size)
{
  char buff [size];

  if (scktType == SCKTUDP)
    {
      status = recvfrom (scktFd, buff, size, 0, (struct sockaddr *) &destAddr, &addrLen);
    }
  else
    {
#if defined (__linux__)
      status = read (scktFd, buff, size);
#elif defined (_WIN32)
      status = recv (scktFd, buff, size, 0);
#endif
    }

  if (status > 0)
    {
      for (unsigned int i = 0; i < size; ++i)
          msg[i] = buff[i];

      status = OK;
    }
  else if (status == ERR)
    {
      if (errno == EAGAIN)
        {
          setInternalStatus ("Receiving timed out.");
          status = TIMEOUT;
        }
      else
        {
          setInternalStatus ("ERROR! No message received.");
        }
    }
  else if (status == CLOSED)
    {
      setInternalStatus ("Connection dropped.");
    }

  return status;
}

int
SenderSckt::sendMsg_Helper (const char *msg, unsigned int size)
{
  if (scktType == SCKTUDP)
    {
      status = sendto (scktFd, msg, size, 0, (struct sockaddr *) &destAddr,
          sizeof (struct sockaddr));
    }
  else
    {
#if defined (__linux__)
      status = write (scktFd, msg, size);
#elif defined (_WIN32)
      status = send (scktFd, msg, size, 0);
#endif
    }

  if (status > 0)
    {
      status = OK;
    }
  else if (status == ERR)
    {
      if (errno == EAGAIN)
        {
          setInternalStatus ("Sending timed out.");
          status = TIMEOUT;
        }
      else
        {
          setInternalStatus ("ERROR! Message didn't reach destination.");
        }
    }
  else if (status == CLOSED)
    {
      setInternalStatus ("Connection dropped.");
    }

  return status;
}
