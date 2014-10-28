/*
 * sessionmanager.h
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef SESSIONMANAGER_H_
#define SESSIONMANAGER_H_

#include <Glacier2/Session.h>
#include "clientmonitor.h"

class SessionManager : public Glacier2::SessionManager
{

public:
  SessionManager (const ClientMonitorPtr& cm, const Ice::LoggerPtr& logger, const bool flag);
  virtual ~SessionManager ();

  virtual Glacier2::SessionPrx create (const std::string& msg,
      const Glacier2::SessionControlPrx& sessionControl, const Ice::Current& curr);

protected:
  const ClientMonitorPtr clientMonitor_Ptr;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;
};

#endif /* SESSIONMANAGER_H_ */
