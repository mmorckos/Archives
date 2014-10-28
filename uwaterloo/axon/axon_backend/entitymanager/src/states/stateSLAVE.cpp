/*
 * stateSLAVE.cpp
 *
 *  Created on: 2012-04-22
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateSLAVE.h"
#include "stateBUSY.h"
#include "stateREG.h"
#include "stateIDLE.h"
#include "stateUNAV.h"
#include "stateENVCONN.h"
#include "../entitymanager.h"
#include "../common/global.h"
#include "../common/status.h"

#include "../util/dataformatconverter.h"
#include "../protobufdata/param.pb.h"
#include "../protobufdata/ctrlparam.pb.h"
#include "../protobufdata/resource.pb.h"
#include "../protobufdata/service.pb.h"

StateSLAVE *StateSLAVE::_instance = 0;

/*! \brief Constructor.
 */
StateSLAVE::StateSLAVE () :
    entry (true)
{}

StateSLAVE*
StateSLAVE::Instance ()
{
  if (_instance == 0)
      _instance = new StateSLAVE;

  return _instance;
}

void
StateSLAVE::exec (EntityManager *manager)
{
  std::vector <std::string> dataSeq;
  std::vector <std::string> replyList;
  std::string msg;
  std::string reply;
  std::string tmp;
  bool taskFlag;
  int status;
  /// If communication with the entity failed go to UNAV state
  if (!manager->getEntityOnline () || !manager->probeStatus (&reply))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("SLAVE", "Alert", "going to state UNAV.");
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  /// If entity manager is not connected to the environment go to ENVCONN state and notify entity
  if (!manager->getConnected ())
    {
      if (!manager->sendEntitySyncMsg (STOP, "Lost connection with environment.", &reply))
        {
          manager->setEntityOnline (false);
          manager->stateLog ("SLAVE", "Alert", "going to state UNAV.");
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          manager->stateLog ("SLAVE", "Alert", "going to state ENVCONN.");
          switchState (manager, StateENVCONN::Instance ());
        }

      return;
    }
  /// Check if entity was released by a client or due to a client abrupt disconnection
  if (manager->getReleased ())
    {
      manager->setInitiator (true);
      manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
      manager->releaseReset ();
      if (!manager->sendEntitySyncMsg (STOP, "Released by controlling client.", &reply))
        {
          manager->setEntityOnline (false);
          manager->stateLog ("SLAVE", "Alert", "going to state UNAV.");
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          manager->stateLog ("SLAVE", "Alert", "going to state IDLE.");
          switchState (manager, StateIDLE::Instance ());
        }

      return;
    }
  /// If there is a pending stop signal send the stop signal to entity
  if (manager->stopSignal (&tmp))
    {
      if (!manager->sendEntitySyncMsg (STOP, tmp, &reply))
        {
          manager->setEntityOnline (false);
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          manager->setInitiator (true);
          manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
          manager->clearPendingEntityTasks ();
          manager->toggleSlaveMode (false);
          manager->stateLog ("SLAVE", "Alert", "going to state REG.");
          switchState (manager, StateREG::Instance ());
        }
      return;
    }
  /// Check if any participating entity dropped
  if (manager->getIncomingMsg (&msg, ENVIR_MSG))
    {
      /// If a participating entity dropped
      if (msg == ENTITY_NOEXIST || msg == ENTITY_DROPPED || msg == ENTITY_RELEASED)
        {
          if (msg == ENTITY_NOEXIST || msg == ENTITY_DROPPED)
            {
              manager->addOutgoingMsg ("Collaboration manager entity disconnected from us.",
                  WARN, CLIENT_MSG);
            }
          manager->setInitiator (true);
          manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
          manager->clearPendingEntityTasks ();
          manager->toggleSlaveMode (false);
          manager->stateLog ("SLAVE", "Alert", "going to state REG.");
          switchState (manager, StateREG::Instance ());
          return;
        }
    }
  /// Check for local tasks and execute them
  if (manager->getPendingTask (&dataSeq, ENTITY_MSG))
    {
      for (unsigned int i = 0; i < dataSeq.size (); ++i)
        {
          status = manager->dispatchToEntity (dataSeq[i], &reply, &taskFlag);
          if (status == ERR)
            {
              manager->setEntityOnline (false);
              switchState (manager, StateUNAV::Instance ());
              return;
            }
          replyList.push_back (reply);
        }
      /// If the entity is accepting the task switch to BUSY
      if (taskFlag)
        {
          manager->stateLog ("SLAVE", "Alert", "going to state BUSY.");
          switchState (manager, StateBUSY::Instance ());
          return;
        }
      /// If there were any data updates add them to outgoing queue
      if (replyList.size () > 0)
          manager->addOutgoingDataSeq (replyList, DATA, CLIENT_MSG);
    }
}
