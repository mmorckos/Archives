/*
 * frontendserver.cpp
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <IceStorm/IceStorm.h>
#include <sstream>

#include "frontendserver.h"
#include "pollsessionmanager_i.h"
#include "entitytofrontendcomm_i.h"
#include "frontendtopics.h"
#include "../common/topicnames.h"

FrontEndServer::FrontEndServer () :
  timer_Ptr (0)
{}

FrontEndServer::~FrontEndServer ()
{}

bool
FrontEndServer::init ()
{
  timer_Ptr = new IceUtil::Timer();

  return true;
}

bool
FrontEndServer::start (int argc, char *argv[], int& status)
{
  ClientMonitorPtr clientMonitor_Ptr;
  Ice::LoggerPtr iceLogger_Ptr;
  ReaperTaskPtr reaperTask_Ptr;
  std::ostringstream oss;
  int timeout;
  bool logFlag;
  IceStorm::TopicManagerPrx topicManager_Prx;
  /* Publisher */
  IceStorm::TopicPrx topicPub_Prx;
  Ice::ObjectPrx publisher;
  FrontEndComm::FrontEndToEntityCommPrx frontEndToEntityComm_Prx;
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
          (communicator ()->propertyToProxy ("TopicManager.Proxy1"));
      if (!topicManager_Prx)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "FrontEndServer: " << "Invalid proxy for topic manager (Publisher).";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      try
        {
          topicPub_Prx = topicManager_Prx->retrieve (SERVER_TO_SERVER_COMM_1);
        }
      catch (const IceStorm::NoSuchTopic& ex)
        {
          try
            {
              topicPub_Prx = topicManager_Prx->create (SERVER_TO_SERVER_COMM_1);
            }
          catch (const IceStorm::TopicExists& ex)
            {
              if (logFlag)
                {
                  oss.str ("");
                  oss << "FrontEndServer: " << "topic already exists (Publisher).";
                  iceLogger_Ptr->trace ("Error", oss.str ());
                }

              return false;
            }
        }

      /// Retrieving the publisher
      publisher = topicPub_Prx->getPublisher ();
      frontEndToEntityComm_Prx = FrontEndComm::FrontEndToEntityCommPrx::uncheckedCast (publisher);
      /* End of publisher handle */

      adapter_Ptr = communicator ()->createObjectAdapter ("FrontEndServer");
      clientMonitor_Ptr = new ClientMonitor (frontEndToEntityComm_Prx, iceLogger_Ptr, logFlag);
/*
      if (!clientMonitor_Ptr->init ())
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "FrontEndServer: " << "ClientMonitor failed to initialize.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
*/
      if (logFlag)
        {
          oss.str ("");
          oss << "FrontEndServer: " << "ClientMonitor started successfully.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }

      /* Subscriber handler */
      topicManager_Prx = IceStorm::TopicManagerPrx::checkedCast
          (communicator ()->propertyToProxy ("TopicManager.Proxy0"));
      while (true)
        {
          try
            {
              topicSub_Prx = topicManager_Prx->retrieve (SERVER_TO_SERVER_COMM_0);
              break;
            }
          catch (const IceStorm::NoSuchTopic& ex)
            {
              if (logFlag)
                {
                  oss.str ("");
                  oss << "FrontEndServer: " << "unable to retrieve topic (Subscriber). Retrying...";
                  iceLogger_Ptr->trace ("Error", oss.str ());
                }

              sleep (1);
            }
        }

      subscriber = adapter_Ptr-> add (new EntityToFrontEndComm_I (clientMonitor_Ptr),
          communicator ()->stringToIdentity ("EntityToFrontEndComm"));
      /// Ensuring that messages will be received in order
      qos["reliability"] = "ordered";

      try
        {
          topicSub_Prx->subscribeAndGetPublisher (qos, subscriber);
        }
      catch(const IceStorm::AlreadySubscribed& ex)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "FrontEndServer: " << "already subscribed (Subscriber).";
              iceLogger_Ptr->trace ("Warning", oss.str ());
            }
        }
      /* End of subscriber handle */

      adapter_Ptr->add (new SessionManager (clientMonitor_Ptr, iceLogger_Ptr, logFlag),
          communicator ()->stringToIdentity ("SessionManager"));

      adapter_Ptr->add (new PollSessionManager_I (clientMonitor_Ptr, reaperTask_Ptr,
          iceLogger_Ptr, logFlag), communicator ()->stringToIdentity ("PollSessionManager"));

      if (logFlag)
        {
          oss.str ("");
          oss << "FrontEndServer: " << "SessionManager started successfully.";
          iceLogger_Ptr->trace ("Info", oss.str ());
        }

      adapter_Ptr->activate ();

      if (logFlag)
        {
          oss.str ("");
          oss << "FrontEndServer: " << "started successfully.";
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
          oss << "FrontEndServer: " << "failed to initialize. Closing...";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      throw;
    }

  status = EXIT_SUCCESS;

  return true;
}

bool
FrontEndServer::stop ()
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
  FrontEndServer frontEndServer;

  frontEndServer.init ();

  return frontEndServer.main (argc, argv);
}
