/*
 * entityserver.cpp
 *
 *  Created on: 2011-12-04
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <IceStorm/IceStorm.h>
#include <sstream>
#include <assert.h>

#include "entityserver.h"
#include "pollsessionmanager_i.h"
#include "frontendtoentitycomm_i.h"
#include "entitytopics.h"
#include "../common/topicnames.h"

EntityServer::EntityServer () :
  timer_Ptr (0)
{}

EntityServer::~EntityServer ()
{}

bool
EntityServer::init ()
{
  timer_Ptr = new IceUtil::Timer();

  return true;
}

bool
EntityServer::start (int argc, char *argv[], int& status)
{
  EntityMonitorPtr entityMonitor_Ptr;
  Ice::LoggerPtr iceLogger_Ptr;
  ReaperTaskPtr reaperTask_Ptr;
  std::ostringstream oss;
  int timeout;
  bool logFlag;
  IceStorm::TopicManagerPrx topicManager_Prx;
  /* Publisher */
  IceStorm::TopicPrx topicPub_Prx;
  Ice::ObjectPrx publisher;
  EntityComm::EntityToFrontEndCommPrx entityToFrontEndComm_Prx;
  /* Subscriber */
  IceStorm::TopicPrx topicSub_Prx;
  Ice::ObjectPrx subscriber;
  IceStorm::QoS qos;

  timeout = communicator ()->getProperties ()->getPropertyAsIntWithDefault ("Reaper_Timeout", 10);
  logFlag = communicator ()->getProperties ()->getPropertyAsIntWithDefault ("Server.Trace", 0) != 0;
  iceLogger_Ptr = communicator ()->getLogger ();
  reaperTask_Ptr = new ReaperTask (timeout, iceLogger_Ptr, logFlag);
  timer_Ptr->scheduleRepeated (reaperTask_Ptr, IceUtil::Time::seconds (timeout));
  try
    {
      /* Publisher handler */
      topicManager_Prx = IceStorm::TopicManagerPrx::checkedCast
          (communicator ()->propertyToProxy ("TopicManager.Proxy0"));
      if (!topicManager_Prx)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityServer: " << "Invalid proxy for topic manager (Publisher).";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      try
        {
          topicPub_Prx = topicManager_Prx->retrieve (SERVER_TO_SERVER_COMM_0);
        }
      catch (const IceStorm::NoSuchTopic& ex)
        {
          try
            {
              topicPub_Prx = topicManager_Prx->create (SERVER_TO_SERVER_COMM_0);
            }
          catch (const IceStorm::TopicExists& ex)
            {
              if (logFlag)
                {
                  oss.str ("");
                  oss << "EntityServer: " << "topic already exists (Publisher).";
                  iceLogger_Ptr->trace ("Warning", oss.str ());
                }

              return false;
            }
        }

      /// Retrieving the publisher
      publisher = topicPub_Prx->getPublisher ();
      entityToFrontEndComm_Prx = EntityComm::EntityToFrontEndCommPrx::uncheckedCast (publisher);
      /* End of publisher handler */
      adapter_Ptr = communicator ()->createObjectAdapter ("EntityServer");
      entityMonitor_Ptr = new EntityMonitor (entityToFrontEndComm_Prx, iceLogger_Ptr, logFlag);
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityServer: " << "EntityMonitor started successfully.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }
      /* Subscriber handler */
      topicManager_Prx = IceStorm::TopicManagerPrx::checkedCast
          (communicator ()->propertyToProxy ("TopicManager.Proxy1"));
      while (true)
        {
          try
            {
              topicSub_Prx = topicManager_Prx->retrieve (SERVER_TO_SERVER_COMM_1);
              break;
            }
          catch (const IceStorm::NoSuchTopic& ex)
            {
              if (logFlag)
                {
                  oss.str ("");
                  oss << "EntityServer: " << "unable to retrieve topic (Subscriber). Retrying...";
                  iceLogger_Ptr->trace ("Error", oss.str ());
                }

              sleep (1);
            }
        }
      subscriber = adapter_Ptr-> add (new FrontEndToEntityComm_I (entityMonitor_Ptr),
          communicator ()->stringToIdentity ("FrontEndToEntityComm"));
      /// Ensuring that messages will be received in order
      qos["reliability"] = "ordered";
      try
        {
          topicSub_Prx->subscribeAndGetPublisher (qos, subscriber);
        }
      catch (const IceStorm::AlreadySubscribed& ex)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityServer: " << "already subscribed (Subscriber).";
              iceLogger_Ptr->trace ("Warning", oss.str ());
            }
        }
      /* End of subscriber handler */
      adapter_Ptr->add (new SessionManager (entityMonitor_Ptr, iceLogger_Ptr, logFlag),
          communicator ()->stringToIdentity ("SessionManager"));
      adapter_Ptr->add (new PollSessionManager_I (entityMonitor_Ptr, reaperTask_Ptr,
          iceLogger_Ptr, logFlag), communicator ()->stringToIdentity ("PollSessionManager"));
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityServer: " << "SessionManager started successfully.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }

      adapter_Ptr->activate ();
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityServer: " << "started successfully.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }
    }
  catch (const Ice::LocalException& ex)
    {
      status = EXIT_FAILURE;
      timer_Ptr->destroy ();

      if (logFlag)
        {
          oss.str ("");
          oss << "EntityServer: " << "failed to initialize. Closing...";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }
      throw;
    }

  status = EXIT_SUCCESS;

  return true;
}

bool
EntityServer::stop ()
{
  timer_Ptr->destroy ();

  return true;
}


/*****************
 * MAIN
 ****************/
int
main(int argc, char *argv[])
{
  EntityServer entityServer;

  entityServer.init ();

  return entityServer.main (argc, argv);
}
