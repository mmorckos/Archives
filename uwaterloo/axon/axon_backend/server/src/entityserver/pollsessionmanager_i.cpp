/*
 * pollsessionmanager_i.cpp
 *
 *  Created on: 2011-12-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <sstream>

#include "pollsessionmanager_i.h"
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

  std::list <std::pair <EntityComm::PollSessionPrx, PollSession_IPtr> >::iterator \
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
ReaperTask::add (const EntityComm::PollSessionPrx& proxy, const PollSession_IPtr& pollSession)
{
  IceUtil::Mutex::Lock sync (mutex);

  reapables.push_back (std::make_pair (proxy, pollSession));
}

/******************************************************/
/******************************************************/

PollSessionManager_I::PollSessionManager_I (const EntityMonitorPtr& em, const ReaperTaskPtr& rp,
    const Ice::LoggerPtr& logger, const bool flag) :
  entityMonitor_Ptr (em),
  reaper_Ptr (rp),
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

PollSessionManager_I::~PollSessionManager_I ()
{}

EntityComm::PollSessionPrx
PollSessionManager_I::create (const std::string& msg, const Ice::Current& curr)
{
  int status;
  std::string entityId;
  EntityComm::PollSessionPrx proxy;

  status = entityMonitor_Ptr->checkRegRequest (msg, &entityId);
  if (status == BADMSG || status == NOTEXIST)
      throw EntityComm::CannotCreatePollSessionException ("Error: Bad message from entity.");

  try
    {
      PollSession_IPtr session = new PollSession_I (entityMonitor_Ptr, entityId,
          iceLogger_Ptr, logFlag);
      proxy = EntityComm::PollSessionPrx::uncheckedCast (curr.adapter->addWithUUID (session));
      reaper_Ptr->add (proxy, session);
    }
  catch (const Ice::ObjectAdapterDeactivatedException& ex)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "PollSessionManager: ";
          oss << "Failed to create session for entity '" << entityId << "'.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw EntityComm::CannotCreatePollSessionException ("Error: Internal server error.");
    }

  if (logFlag)
    {
      std::ostringstream oss;

      oss << "PollSessionManager: session created for '" << entityId << "'.";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }

  return proxy;
}
