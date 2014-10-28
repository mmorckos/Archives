/*
 * stateBUSY.cpp
 *
 *  Created on: 2012-02-09
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateBUSY.h"
#include "stateCOLLAB.h"
#include "stateSLAVE.h"
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

StateBUSY *StateBUSY::_instance = 0;

/*! \brief Constructor.
 */
StateBUSY::StateBUSY () :
    taskStatus (FAILURE)
{}

StateBUSY*
StateBUSY::Instance ()
{
  if (_instance == 0)
      _instance = new StateBUSY;

  return _instance;
}

/*! \brief This function houses the main logic of the state.
 *
 * The UNAV state will attempt to start up entity's process AND/OR establish connection with the
 * entity. If successful, it will shift to the IDLE state.
 */
void
StateBUSY::exec (EntityManager *manager)
{
  std::string tmp;
  std::string msg;
  std::string reply;
  char replyBuff [BUFFSIZE];
  std::vector <std::string> dataSeq;
  std::vector <std::string> replyList;
  int status;
  bool flag;

  status = 0;
  if (!manager->getEntityOnline () || !manager->probeStatus (&tmp))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("BUSY", "Alert", "going to state UNAV.");
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  /// If the entity has recently completed a TASK and is now ready send its feedback to the client
  else if (tmp == READY)
    {
      if (getTaskFdbk (manager, &replyList) == ERR)
        {
          manager->setEntityOnline (false);
          manager->stateLog ("BUSY", "Alert", "going to state UNAV.");
          switchState (manager, StateUNAV::Instance ());
          return;
        }
      /// Send TASK feedback to client and go back to state REG
      else
        {
          if (replyList.size () > 0)
              manager->addOutgoingDataSeq (replyList, DATA, CLIENT_MSG);

          if (manager->collaborationMode ())
            {
              manager->addIncomingMsg (taskStatus, ENTITY_MSG);
              manager->stateLog ("BUSY", "Alert", "going to state COLLAB.");
              switchState (manager, StateCOLLAB::Instance ());
            }
          else if (manager->slaveMode ())
            {
              manager->addOutgoingEntityMsg (taskStatus, INFO, manager->getCollaboratorUuid ());
              manager->stateLog ("BUSY", "Alert", "going to state SLAVE.");
              switchState (manager, StateSLAVE::Instance ());
            }
          else
            {
              manager->stateLog ("BUSY", "Alert", "going to state REG.");
              switchState (manager, StateREG::Instance ());
            }

          return;
        }
    }
  /// If entity manager is not connected to the environment go to ENVCONN state and notify entity
  if (!manager->getConnected ())
    {
      if (manager->collaborationMode ())
        {
          manager->stateLog ("BUSY", "Alert", "going to state COLLAB.");
          switchState (manager, StateCOLLAB::Instance ());
        }
      else if (manager->slaveMode ())
        {
          manager->stateLog ("BUSY", "Alert", "going to state SLAVE.");
          switchState (manager, StateSLAVE::Instance ());
        }
      else
        {
          if (!manager->sendEntitySyncMsg (STOP, "Lost connection with environment.", &reply))
            {
              manager->setEntityOnline (false);
              manager->stateLog ("BUSY", "Alert", "going to state UNAV.");
              switchState (manager, StateUNAV::Instance ());
            }
          else
            {
              manager->stateLog ("BUSY", "Alert", "going to state ENVCONN.");
              switchState (manager, StateENVCONN::Instance ());
            }
        }

      return;
    }
  /// Check if entity was released by a client or due to a client abrupt disconnection
  if (manager->getReleased ())
    {
      if (manager->collaborationMode ())
        {
          manager->stateLog ("BUSY", "Alert", "going to state COLLAB.");
          switchState (manager, StateCOLLAB::Instance ());
        }
      else if (manager->slaveMode ())
        {
          manager->stateLog ("BUSY", "Alert", "going to state SLAVE.");
          switchState (manager, StateSLAVE::Instance ());
        }
      else
        {
          manager->releaseReset ();

          if (!manager->sendEntitySyncMsg (STOP, "Released by controlling client.", &reply))
            {
              manager->setEntityOnline (false);
              manager->stateLog ("BUSY", "Alert", "going to state UNAV.");
              switchState (manager, StateUNAV::Instance ());
            }
          else
            {
              manager->stateLog ("BUSY", "Alert", "going to state IDLE.");
              switchState (manager, StateIDLE::Instance ());
            }
        }

      return;
    }
  /// If there is a pending stop signal send the stop signal to entity
  if (manager->stopSignal (&tmp))
    {
      /// If we are in collaboration or slave mode
      if (manager->collaborationMode ())
        {
          switchState (manager, StateCOLLAB::Instance ());
        }
      else if (manager->slaveMode ())
        {
          switchState (manager, StateSLAVE::Instance ());
        }
      else
        {
          if (!manager->sendEntitySyncMsg (STOP, tmp, &reply))
            {
              manager->setEntityOnline (false);
              switchState (manager, StateUNAV::Instance ());
              return;
            }
          switchState (manager, StateREG::Instance ());
          /// Reset the stop signal
          manager->toggleStop (false);
        }

      return;
    }

  tmp = "";
  /// If there is a position update from the entity
  if (!manager->sendEntitySyncMsg (POS, " ", replyBuff, BUFFSIZE))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("BUSY", "Alert", "going to state UNAV.");
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  else if (strcmp (replyBuff, NONE.c_str ()))
    {
      Data::Position position;
      DataFormatConverter ft;

      if (ft.formatPosition (&position, replyBuff, BUFFSIZE, false)
          && ft.formatPosition (&position, &tmp, true))
        {
          dataSeq.push_back (tmp);
        }
      else
        {
          manager->stateLog ("BUSY", "Error", "Failed to parse position data.");
        }
    }

  reply = "";
  /// Checking for general messages for client from the entity
  if (!manager->sendEntitySyncMsg (CLIENT_INFOMSG, " ", &reply))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("BUSY", "Alert", "going to state UNAV.");
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  else
    {
      /// If there is an info client message relay to controlling client
      if (reply != NONE)
          manager->addOutgoingMsg (reply, INFO, CLIENT_MSG);
    }
  /// Check client data sequence queue for data batches
  if (manager->getIncomingDataSeq (&dataSeq, CLIENT_MSG))
    {
      for (unsigned int i = 0; i < dataSeq.size (); ++i)
        {
          status = manager->dispatchToEntity (dataSeq[i], &reply, &flag);
          if (status == ERR)
            {
              manager->setEntityOnline (false);
              switchState (manager, StateUNAV::Instance ());
              return;
            }
           replyList.push_back (reply);
        }
    }

  if (replyList.size () > 0)
      manager->addOutgoingDataSeq (replyList, DATA, CLIENT_MSG);
  /// If in any special mode check for environment updates
  if (manager->collaborationMode () || manager->slaveMode ())
    {
      if (manager->getIncomingMsg (&msg, ENVIR_MSG))
        {
          /// If a participating entity dropped
          if (msg == ENTITY_NOEXIST || msg == ENTITY_DROPPED || msg == ENTITY_RELEASED)
            {
              if (!manager->sendEntitySyncMsg (STOP, tmp, &reply))
                {
                  manager->setEntityOnline (false);
                  manager->stateLog ("BUSY", "Alert", "going to state UNAV.");
                  switchState (manager, StateUNAV::Instance ());
                }
              else if (manager->collaborationMode ())
                {
                  manager->toggleCollaborationMode (false);
                  manager->stateLog ("BUSY", "Alert", "going to state COLLAB.");
                  switchState (manager, StateCOLLAB::Instance ());
                }
              else if (manager->slaveMode ())
                {
                  manager->toggleSlaveMode (false);
                  manager->stateLog ("BUSY", "Alert", "going to state SLAVE.");
                  switchState (manager, StateSLAVE::Instance ());
                }
            }
        }
    }
}

int
StateBUSY::getTaskFdbk (EntityManager *manager, std::vector <std::string> *replyList)
{
  DataFormatConverter ft;
  char dataBuff [BUFFSIZE];
  std::string tmp;
  Data::Task task;
  /// Get the recently completed TASK from the entity (which will enclose its status)
  if (!manager->sendEntitySyncMsg (TSK_FDBK, " ", dataBuff, BUFFSIZE))
    {
      return ERR;
    }

  if (!ft.formatTask (&task, dataBuff, BUFFSIZE, false) || !ft.formatTask (&task, &tmp, true))
    {
      return FALSE;
    }
  else
    {
      taskStatus = task.status () == Data::Task_Status_SUCCESS ? SUCCESS : FAILURE;
      replyList->push_back (tmp);
      return OK;
    }
}
