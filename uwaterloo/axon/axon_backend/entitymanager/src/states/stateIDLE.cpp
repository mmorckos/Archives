/*
 * stateREADY.cpp
 *
 *  Created on: 2011-11-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateIDLE.h"
#include "stateREG.h"
#include "stateENVCONN.h"
#include "stateUNAV.h"
#include "../entitymanager.h"
#include "../common/global.h"
#include "../common/status.h"

StateIDLE *StateIDLE::_instance = 0;

/*! \brief Constructor.
 */
StateIDLE::StateIDLE ()
{}

StateIDLE*
StateIDLE::Instance ()
{
  if (_instance == 0)
      _instance = new StateIDLE;

  return _instance;
}

/*! \brief This function houses the main logic of the state.
 *
 * The IDLE state will attempt to establish connection with the environment. If successful, it
 * will keep listening to potential clients.
 */
void
StateIDLE::exec (EntityManager *manager)
{
  std::string msg;
  std::string tmp;
  /// If communication with the entity failed go to UNAV state
  if (!manager->getEntityOnline () || !manager->probeStatus (&tmp))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("IDLE", "Alert", "going to state UNAV.");
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  /// If entity manager is not connected to the environment go to ENVCONN state and notify entity
  if (!manager->getConnected ())
    {
      if (!manager->sendEntitySyncMsg (STOP, "Lost connection with environment.", &msg))
        {
          manager->setEntityOnline (false);
          manager->stateLog ("IDLE", "Alert", "going to state UNAV.");
          switchState (manager, StateUNAV::Instance ());
        }
      else
        {
          manager->stateLog ("IDLE", "Alert", "going to state ENVCONN.");
          switchState (manager, StateENVCONN::Instance ());
        }

      return;
    }
  /// Check for any requests from clients. If there is a request go to REG state.
  if (manager->getRequested ())
    {
      std::string tmp;
      /// If communication with the entity failed go to UNAV state
      if (!manager->sendEntitySyncMsg(CHECKOUT_ENTITY, " ", &tmp))
        {
          manager->setEntityOnline (false);
          manager->stateLog ("IDLE", "Alert", "going to state UNAV.");
          switchState (manager, StateUNAV::Instance ());
          return;
        }
      /// If entity granted registration go to REG state
      if (tmp == OK_CHECKOUT)
        {
          /// Signal manager to compile service records and dispatch them to controlling client
          manager->compileSvcProfile ();
          manager->setRequested (false);
          manager->setCheckedOut (true);
          manager->stateLog ("IDLE", "Alert", "going to state REG.");
          switchState (manager, StateREG::Instance ());
        }
    }
}
