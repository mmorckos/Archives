/*
 * stateCOLLAB.cpp
 *
 *  Created on: 2012-04-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateCOLLAB.h"
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

StateCOLLAB *StateCOLLAB::_instance = 0;

/*! \brief Constructor.
 */
StateCOLLAB::StateCOLLAB () :
    currFulfillment (-1),
    entityCount (0),
    entry (true),
    ready (false)
{}

StateCOLLAB*
StateCOLLAB::Instance ()
{
  if (_instance == 0)
      _instance = new StateCOLLAB;

  return _instance;
}

/*! \brief This function houses the main logic of the state.
 *
 * The UNAV state will attempt to start up entity's process AND/OR establish connection with the
 * entity. If successful, it will shift to the IDLE state.
 */
void
StateCOLLAB::exec (EntityManager *manager)
{
  std::string msg;
  std::string reply;
  std::string tmp;
  std::string id;

  /// If communication with the entity failed go to UNAV state
  if (!manager->getEntityOnline () || !manager->probeStatus (&reply))
    {
      manager->setEntityOnline (false);
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  /// If entity manager is not connected to the environment go to ENVCONN state and notify entity
  if (!manager->getConnected ())
    {
      reset ();

      if (!manager->sendEntitySyncMsg (STOP, "Lost connection with environment.", &reply))
        {
          manager->setEntityOnline (false);
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          switchState (manager, StateENVCONN::Instance ());
        }

      return;
    }
  /// Check if entity was released by a client or due to a client abrupt disconnection
  if (manager->getReleased ())
    {
      reset ();
      manager->setInitiator (true);
      manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
      manager->stateLog ("COLLAB", "Alert", "Released by client. Terminating all collaboration.");
      manager->releaseReset ();
      reset ();

      if (!manager->sendEntitySyncMsg (STOP, "Released by controlling client.", &reply))
        {
          manager->setEntityOnline (false);
          manager->stateLog ("COLLAB", "Alert", "going to state UNAV.");
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          manager->stateLog ("COLLAB", "Alert", "going to state IDLE.");
          switchState (manager, StateIDLE::Instance ());
        }

      return;
    }
  /// If there is a pending stop signal send the stop signal to entity
  if (manager->stopSignal (&tmp))
    {
      reset ();

      if (!manager->sendEntitySyncMsg (STOP, tmp, &reply))
        {
          manager->setEntityOnline (false);
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          reset ();
          manager->setInitiator (true);
          manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
          manager->addOutgoingMsg ("Received a stop signal. Terminating all collaboration.",
              "Warning", CLIENT_MSG);
          manager->clearPendingClientCollaborativeTasks ();
          manager->toggleCollaborationMode (false);
          manager->stateLog ("COLLAB", "Alert", "going to state REG.");
          switchState (manager, StateREG::Instance ());
        }

      return;
    }
  /// If collaboration mode was toggles off by another state
  if (!manager->collaborationMode ())
    {
      reset ();
      manager->setInitiator (true);
      manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
      manager->addOutgoingMsg ("Could not perform one or more services.", "Error", CLIENT_MSG);
      manager->clearPendingClientCollaborativeTasks ();
      manager->toggleCollaborationMode (false);
      manager->stateLog ("COLLAB", "Alert", "going to state REG.");
      switchState (manager, StateREG::Instance ());
      return;
    }
  /// If first time to enter state
  if (entry)
    {
      if (!manager->getPendingCollaborativeTask (&collabTaskData, CLIENT_MSG))
        {
          manager->stateLog ("COLLAB", "Warning", "No pending collaborative tasks.");
          switchState (manager, StateREG::Instance ());
          return;
        }
      /// Add controlling client ID (for authenticaltion)
      collaborators.push_back (manager->getClientUuid ());
      /// Scan all tasks collecting precedence data and informing participating entities
      for (unsigned int i = 0; i < collabTaskData.dataList.size (); ++i)
        {
          tmp = collabTaskData.dataList[i][0];
          id = collabTaskData.dataList[i][1];
          /// Store all precendencies
          if (fulfillmentMap.find (tmp) == fulfillmentMap.end ())
              fulfillmentMap [tmp] = 1;
          else
              fulfillmentMap [tmp] += 1;
          /// If this is not the local entity inform it of the collaboration
          if (id != manager->getUuid ())
              collaborators.push_back (id);
        }
      /// Send participating entities information to environment to initial collaboration
      if (collaborators.size () > 1)
        {
          manager->addOutgoingDataSeq (collaborators, COLLABORATION, ENVIR_MSG);
        }
      /// There are no collaborating entities
      else
        {
          reset ();
          manager->addOutgoingMsg ("Requested task is not recognized as collaborative.", WARN,
              CLIENT_MSG);
          manager->clearPendingClientCollaborativeTasks ();
          manager->toggleCollaborationMode (false);
          manager->stateLog ("COLLAB", "Alert", "going to state REG.");
          switchState (manager, StateREG::Instance ());
          return;
        }

      entry = false;
    }
  /// Check if any participating entity dropped
  if (manager->getIncomingMsg (&msg, ENVIR_MSG))
    {
      /// If a participating entity dropped
      if (msg == ENTITY_NOEXIST || msg == ENTITY_DROPPED || msg == ENTITY_RELEASED)
        {
          reset ();
          manager->setInitiator (true);
          manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
          manager->addOutgoingMsg ("Lost connection with one or more participating entities.",
              "Error", CLIENT_MSG);
          manager->clearPendingClientCollaborativeTasks ();
          manager->toggleCollaborationMode (false);
          manager->stateLog ("COLLAB", "Alert", "going to state REG.");
          switchState (manager, StateREG::Instance ());
          return;
        }
    }
  /// If all is set carry on with task execution
  if (ready)
    {
      execTasks (manager);
    }
  /// Check for confirmation messages from entities
  else if (manager->getIncomingMsg (&msg, ENTITY_MSG))
    {
      if (msg == COLLABORATION_OK)
        {
          ++entityCount;
          if (entityCount == collaborators.size () - 1)
              ready = true;
        }
    }
}

void
StateCOLLAB::execTasks (EntityManager *manager)
{
  std::vector <std::string> dataSeq;
  std::vector <std::string> replyList;
  std::string reply;
  bool taskFlag;
  int status;

  if (currPreced == "" || currFulfillment == fulfillmentMap[currPreced])
    {
      /// If there are still remaining tasks
      if (collabTaskData.dataList.size () > 0)
        {
          currPreced = collabTaskData.dataList[0][0];
          currFulfillment = 0;
          /// Collect all tasks of same precedence and dispatch them
          while (collabTaskData.dataList.size () > 0)
            {
              std::vector <std::string> taskData;
              std::vector <std::string>::iterator it;
              /// Stop when a preced change is detected since we already have dispatched all
              /// its tasks
              if (currPreced != collabTaskData.dataList[0][0])
                  break;

              for (unsigned int j = 2; j < collabTaskData.dataList[0].size (); ++j)
                  taskData.push_back (collabTaskData.dataList[0][j]);
              /// If this task is for the local entity
              if (collabTaskData.dataList[0][1] == manager->getUuid ())
                  manager->addNewTask (taskData, ENTITY_MSG);
              /// If this task is for a participating entity
              else
                  manager->addOutgoingEntityDataSeq (taskData, TSK, collabTaskData.dataList[0][1]);
              /// Erase the head of the list
              collabTaskData.dataList.erase (collabTaskData.dataList.begin ());
            }
        }
      /// Report to client that the task is done and terminate collaboration
      else
        {
          reset ();
          manager->setInitiator (true);
          manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
          manager->addOutgoingMsg ("Collaborative service execution is success.", INFO,
              CLIENT_MSG);
          manager->toggleCollaborationMode (false);
          manager->stateLog ("COLLAB", "Alert", "going to state REG.");
          switchState (manager, StateREG::Instance ());
          return;
        }
    }
  /// Check progress so far
  if (!monitorProgress (manager))
    {
      reset ();
      manager->setInitiator (true);
      manager->addOutgoingMsg (TERMINATE_COLLABORATION, REQUEST, ENVIR_MSG);
      manager->addOutgoingMsg ("Failed to perform one or more services.",
          "Error", CLIENT_MSG);
      manager->clearPendingClientCollaborativeTasks ();
      manager->toggleCollaborationMode (false);
      manager->stateLog ("COLLAB", "Alert", "going to state REG.");
      switchState (manager, StateREG::Instance ());
    }
  /// Check for local tasks and execute them
  else if (manager->getPendingTask (&dataSeq, ENTITY_MSG))
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
          manager->stateLog ("COLLAB", "Alert", "going to state BUSY.");
          switchState (manager, StateBUSY::Instance ());
          return;
        }
      /// If there were any data updates add them to outgoing queue
      if (replyList.size () > 0)
          manager->addOutgoingDataSeq (replyList, DATA, CLIENT_MSG);
    }
}

bool
StateCOLLAB::monitorProgress (EntityManager *manager)
{
  std::string msg;
  /// Check for entity messages
  if (manager->getIncomingMsg (&msg, ENTITY_MSG))
    {
      /// If task is success increase fulfillment count
      if (msg == SUCCESS)
        {
          ++currFulfillment;
          return true;
        }
      else
        {
          return false;
        }
    }

  return true;
}

void
StateCOLLAB::reset ()
{
  collabTaskData.dataList.clear ();
  collaborators.clear ();
  fulfillmentMap.clear ();
  currPreced = "";
  currFulfillment = -1;
  entityCount = 0;
  entry = true;
  ready = false;
}
