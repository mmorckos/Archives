/*
 * sessionmanager.cpp
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "sessionmanager.h"
#include "session_i.h"
#include "../common/status.h"

SessionManager::SessionManager (const ClientMonitorPtr& em, const Ice::LoggerPtr& logger,
    const bool flag) :
  clientMonitor_Ptr (em),
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

SessionManager::~SessionManager ()
{}

Glacier2::SessionPrx
SessionManager::create (const std::string& msg,
    const Glacier2::SessionControlPrx& sessionControl, const Ice::Current& curr)
{
  Glacier2::SessionPrx proxy;
  int status;
  std::string clientId;

  status = clientMonitor_Ptr->checkRegRequest (msg, &clientId);
  if (status == BADMSG || status == NOTEXIST)
      throw Glacier2::CannotCreateSessionException ("Error: Bad message from entity.");

  try
    {
      Ice::IdentitySeq idSeq;
      Session_IPtr session = new Session_I (clientMonitor_Ptr, clientId, iceLogger_Ptr, logFlag);

      proxy = Glacier2::SessionPrx::uncheckedCast (curr.adapter->addWithUUID (session));

      idSeq.push_back (proxy->ice_getIdentity ());
      sessionControl->identities ()->add (idSeq);
    }
  catch (const Ice::LocalException& ex)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "SessionManager: Failed to create session for client '" << clientId << "'.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      if (proxy)
          proxy->destroy ();

      throw Glacier2::CannotCreateSessionException ("Error: Internal server error.");
    }

  if (logFlag)
    {
      std::ostringstream oss;

      oss << "SessionManager: session created for '" << clientId << "'.";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }

  return proxy;
}
