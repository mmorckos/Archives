/*
 * pollsessionmanager_i.cpp
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "pollsessionmanager_i.h"
#include "../common/global.h"
#include "../common/status.h"

ReaperTask::ReaperTask (int tmo, const Ice::LoggerPtr& logger, const bool flag) :
  timeout (IceUtil::Time::seconds (tmo)),
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

ReaperTask::~ReaperTask ()
{}

void
ReaperTask::runTimerTask ()
{
  IceUtil::Mutex::Lock sync (mutex);

  std::list <std::pair <FrontEndComm::PollSessionPrx, PollSession_IPtr> >::iterator \
  it = reapables.begin();

  while (it != reapables.end ())
    {
      try
        {
          if ((IceUtil::Time::now (IceUtil::Time::Monotonic) - it->second->timestamp ()) > timeout)
            {
              if (logFlag)
                {
                  std::ostringstream oss;

                  oss << "ReaperTask: " << "'" << it->first << " is terminated.";
                  iceLogger_Ptr->trace ("Info", oss.str ());
                }

              it->first->destroy ();
              it = reapables.erase (it);
            }
          else
            {
              ++it;
            }
        }
      catch (const Ice::LocalException& ex)
        {
          it = reapables.erase (it);
        }
    }
}

void
ReaperTask::add (const FrontEndComm::PollSessionPrx& proxy, const PollSession_IPtr& pollSession)
{
  IceUtil::Mutex::Lock sync (mutex);

  reapables.push_back (std::make_pair (proxy, pollSession));
}

/******************************************************/
/******************************************************/

PollSessionManager_I::PollSessionManager_I (const ClientMonitorPtr& cm,
    const ReaperTaskPtr& rp, const Ice::LoggerPtr& logger, const bool flag) :
  clientMonitor_Ptr (cm),
  reaper_Ptr (rp),
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

PollSessionManager_I::~PollSessionManager_I ()
{}

FrontEndComm::PollSessionPrx
PollSessionManager_I::create (const std::string& msg, const Ice::Current& curr)
{
  int status;
  std::string clientId;
  FrontEndComm::PollSessionPrx proxy;

  status = clientMonitor_Ptr->checkRegRequest (msg, &clientId);
  if (status == BADMSG || status == NOTEXIST)
      throw FrontEndComm::CannotCreatePollSessionException ("Error: Bad message from client.");

  try
    {
      PollSession_IPtr session = new PollSession_I (clientMonitor_Ptr, clientId, iceLogger_Ptr,
          logFlag);
      proxy = FrontEndComm::PollSessionPrx::uncheckedCast (curr.adapter->addWithUUID (session));
      reaper_Ptr->add (proxy, session);
    }
  catch (const Ice::ObjectAdapterDeactivatedException& ex)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "PollSessionManager: ";
          oss << "Failed to create session for client '" << clientId << "'.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw FrontEndComm::CannotCreatePollSessionException ("Error: Internal server error.");
    }

  if (logFlag)
    {
      std::ostringstream oss;

      oss << "PollSessionManager: session created for '" << clientId << "'.";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }

  return proxy;
}
