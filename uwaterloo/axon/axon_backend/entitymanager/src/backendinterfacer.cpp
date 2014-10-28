/*
 * backendinterfacer.cpp
 *
 *  Created on: 2012-01-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <stdio.h>

#include "backendinterfacer.h"
#if defined (__linux__)
#include "./common/global.h"
#include "./common/status.h"
#include "./util/configparser.h"
#elif defined (_WIN32)
#include "..\\common\\common.h"
#include "..\\common\\entities.h"
#include "..\\util\\configparser.h"
#endif

BackEndInterfacer::BackEndInterfacer (const Ice::LoggerPtr& logger, const bool& flag) :
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

BackEndInterfacer::~BackEndInterfacer ()
{}

bool
BackEndInterfacer::init (const std::string& fileName)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  Util::ConfigParser parser;
  std::map <std::string, std::string> parsedData;
  std::map <std::string, std::string>::iterator itr;
  unsigned int porta0;
  unsigned int porta1;
  long sendTimeout;
  long recvTimeout;
  bool proto;
  std::string hostName;
  unsigned int count;

  porta0 = 0;
  porta1 = 0;
  sendTimeout = 0;
  recvTimeout = 0;
  proto = 0;
  count = 0;
  if (!parser.setFile (fileName))
    {
      if (logFlag)
        {
          iceLogger_Ptr->trace ("Error",
              "BackEndInterfacer: port config file could not be open. File does not exits.");
        }

      return false;
    }

  if (!parser.parse (&parsedData))
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Error", "BackEndInterfacer: file is not valid.");

      return false;
    }

  for (itr = parsedData.begin (); itr != parsedData.end (); ++itr)
    {
      if ((*itr).first == "PORTa0")
        {
          porta0 = atoi (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "PORTa1")
        {
          porta1 = atoi (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "SNDTMO")
        {
          sendTimeout = atol (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "RECVTMO")
        {
          recvTimeout = atol (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "PROTO")
        {
          proto = atoi (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "HOST")
        {
          hostName = (*itr).second;
          ++count;
        }
      else
        {
          if (logFlag)
            {
              iceLogger_Ptr->trace ("Warning",
                  "BackEndInterfacer: unknown configuration parameter.");
            }
        }
    }

  if (count != CFG_PARAM_COUNT)
    {
      if (logFlag)
        {
          iceLogger_Ptr->trace ("Error",
              "BackEndInterfacer: missing port configuration parameters.");
        }

      return false;
    }
  /// Setting port numbers range
  if (!commSckt.setPortRange (porta0, porta1))
    {
      if (logFlag)
        {
          iceLogger_Ptr->trace ("Error",
              "BackEndInterfacer: failed to set port numbers range. Invalid range.");
        }

      return false;
    }
  /// Setting host name
  if (!commSckt.setIpAddr (hostName))
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Error", "invalid hostname");

      return false;
    }
  /// Initializing sockets
  if (!commSckt.init (proto, recvTimeout, sendTimeout))
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Error", "BackEndInterfacer: failed to initialize sockets.");

      return false;
    }

  if (logFlag)
      iceLogger_Ptr->trace ("Info", "BackEndInterfacer: sockets initialization is success.");

  return true;
}

bool
BackEndInterfacer::openConn ()
{
  int tmp0;

  if (logFlag)
      iceLogger_Ptr->trace ("Info", "BackEndInterfacer: Listening for an entity...");

  tmp0 = commSckt.openConn ();
  if (tmp0 != ERR)
    {
      if (logFlag)
          iceLogger_Ptr->trace ("Info", "BackEndInterfacer: Connecting to an entity.");

      return true;
    }

  if (logFlag)
      iceLogger_Ptr->trace ("Error", "BackEndInterfacer: Failed to connect to entity.");

  return false;
}

void
BackEndInterfacer::closeConn ()
{
  commSckt.closeConn ();
}

bool
BackEndInterfacer::sendSyncMsg (const std::string& msg, std::string *reply)
{
  if (commSckt.sendMsg (msg) != OK || commSckt.getMsg (reply) != OK)
      return false;

  return true;
}

bool
BackEndInterfacer::sendSyncMsg (const std::string& msg, char *reply, unsigned int size)
{
  if (commSckt.sendMsg (msg) != OK || commSckt.getMsg (reply, size) != OK)
      return false;

  return true;
}

bool
BackEndInterfacer::sendSyncMsg (const char *msg, unsigned int size0, char *reply,
    unsigned int size1)
{
  if (commSckt.sendMsg (msg, size0) != OK || commSckt.getMsg (reply, size1) != OK)
      return false;

  return true;
}
