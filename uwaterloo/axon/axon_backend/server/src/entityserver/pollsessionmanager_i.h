/*
 * pollsessionmanager_i.h
 *
 *  Created on: 2011-12-12
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef POLLSESSIONMANAGER_I_H_
#define POLLSESSIONMANAGER_I_H_

#include <list>
#include <Ice/Ice.h>

#include "entitycomm.h"
#include "pollsession_i.h"
#include "entitymonitor.h"

class ReaperTask : public IceUtil::TimerTask
{

public:
    ReaperTask (int tmo, const Ice::LoggerPtr& logger, const bool flag);
    virtual ~ReaperTask ();

    virtual void runTimerTask ();
    void add (const EntityComm::PollSessionPrx& proxy, const PollSession_IPtr& pollSession);

protected:
    const IceUtil::Time timeout;
    Ice::LoggerPtr iceLogger_Ptr;
    std::list <std::pair <EntityComm::PollSessionPrx, PollSession_IPtr> > reapables;
    IceUtil::Mutex mutex;
    bool logFlag;
};
typedef IceUtil::Handle <ReaperTask> ReaperTaskPtr;

/******************************************************/
/******************************************************/

class PollSessionManager_I : public EntityComm::PollSessionManager
{

public:
  PollSessionManager_I (const EntityMonitorPtr& em, const ReaperTaskPtr& rp,
      const Ice::LoggerPtr& logger, const bool flag);
  virtual ~PollSessionManager_I ();

  virtual EntityComm::PollSessionPrx create (const std::string& msg,
      const Ice::Current& curr);

protected:
  const EntityMonitorPtr entityMonitor_Ptr;
  const ReaperTaskPtr reaper_Ptr;
  const Ice::LoggerPtr iceLogger_Ptr;
  const bool logFlag;
};

#endif /* POLLSESSIONMANAGER_I_H_ */
