/*
 * stateREG.cpp
 *
 *  Created on: 2012-02-09
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateREG.h"
#include "stateCOLLAB.h"
#include "stateSLAVE.h"
#include "stateIDLE.h"
#include "stateBUSY.h"
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
#include "../protobufdata/task.pb.h"

StateREG *StateREG::_instance = 0;

/*! \brief Constructor.
 */
StateREG::StateREG () :
    taskFlag (false)
{}

StateREG*
StateREG::Instance ()
{
  if (_instance == 0)
      _instance = new StateREG;

  return _instance;
}

/*! \brief This function houses the main logic of the state.
 *
 * The UNAV state will attempt to start up entity's process AND/OR establish connection with the
 * entity. If successful, it will shift to the IDLE state.
 */
void
StateREG::exec (EntityManager *manager)
{
  std::string tmp;
  std::string reply;
  char replyBuff [BUFFSIZE];
  std::vector <std::string> dataSeq;
  std::vector <std::string> replyList;
  int status;

  status = 0;
  taskFlag = false;

  if (!manager->getEntityOnline () || !manager->probeStatus (&reply))
    {
      manager->setEntityOnline (false);
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  /// If entity manager is not connected to the environment go to ENVCONN state and notify entity
  if (!manager->getConnected ())
    {
      if (!manager->sendEntitySyncMsg (STOP, "Lost connection with environment.", &reply))
        {
          manager->setEntityOnline (false);
          switchState (manager, StateUNAV::Instance ());
          manager->stateLog ("REG", "Alert", "going to state UNAV.");
        }
      else
        {
          switchState (manager, StateENVCONN::Instance ());
          manager->stateLog ("REG", "Alert", "going to state ENVCONN.");
        }

      return;
    }
  /// Check if entity was released by a client or due to a client abrupt disconnection
  if (manager->getReleased ())
    {
      manager->releaseReset ();

      if (!manager->sendEntitySyncMsg (STOP, "Released by controlling client.", &reply))
        {
          manager->setEntityOnline (false);
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          manager->stateLog ("REG", "Alert", "going to state IDLE.");
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
          manager->stateLog ("REG", "Alert", "going to state UNAV.");
          switchState (manager, StateUNAV::Instance ());
          return;
        }
      /// Reset the stop signal
      manager->toggleStop (false);
    }

  tmp = "";
  /// If there is a position update from the entity
  if (!manager->sendEntitySyncMsg (POS, " ", replyBuff, BUFFSIZE))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("REG", "Alert", "going to state UNAV.");
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
          manager->stateLog ("REG", "Error", "Failed to parse position data.");
        }
    }

  reply = "";
  /// Polling entity for pending client messages
  if (!manager->sendEntitySyncMsg (CLIENT_INFOMSG, " ", &reply))
    {
      manager->setEntityOnline (false);
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
          status = manager->dispatchToEntity (dataSeq[i], &reply, &taskFlag);

          if (status == ERR)
            {
              manager->setEntityOnline (false);
              switchState (manager, StateUNAV::Instance ());
              return;
            }

           replyList.push_back (reply);
        }
    }
  /// Switch to COLLAB state if there is a pending collaborative task(s)
  if (manager->collaborationMode ())
    {
      manager->stateLog ("REG", "Alert", "going to state COLLAB.");
      switchState (manager, StateCOLLAB::Instance ());
      return;
    }
  /// Switch to SLAVE state if there is a pending request
  if (manager->slaveMode ())
    {
      manager->stateLog ("REG", "Alert", "going to state SLAVE.");
      switchState (manager, StateSLAVE::Instance ());
      return;
    }
  /// Check client data sequence queue for pending tasks
  if (manager->getPendingTask (&dataSeq, CLIENT_MSG))
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
          manager->stateLog ("REG", "Alert", "going to state BUSY.");
          reset ();
          switchState (manager, StateBUSY::Instance ());
          return;
        }
    }
  /// If there were any data updates add them to outgoing queue
  if (replyList.size () > 0)
      manager->addOutgoingDataSeq (replyList, DATA, CLIENT_MSG);
}

void
StateREG::reset ()
{
  taskFlag = false;
}

/*! \brief This function splits a message into a prefix and a body.
 *
 * \param prefix of type string*, which will store the message's prefix.
 * \param msg of type string*, which will store the message's body.
 * \return true if the message is properly formatted. false otherwise.
 */
bool
StateREG::extractMsg (const std::string& msg, std::string *prefix, std::string *body)
{
  size_t pos;

  pos = msg.find (DELIM);

  if (pos == std::string::npos)
      return false;

  (*prefix) = msg.substr (0, pos);
  (*body) = msg.substr (pos + 1);

  if (prefix->size () == 0 || body->size () == 0)
      return false;

  return true;
}
