/*
 * stateENVCONN.cpp
 *
 *  Created on: 2012-01-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateENVCONN.h"
#include "stateIDLE.h"
#include "stateUNAV.h"
#include "stateERR.h"
#include "../entitymanager.h"
#include "../common/global.h"
#include "../common/status.h"

StateENVCONN *StateENVCONN::_instance = 0;

/*! \brief Constructor.
 */
StateENVCONN::StateENVCONN ()
{}

StateENVCONN*
StateENVCONN::Instance ()
{
  if (_instance == 0)
      _instance = new StateENVCONN;

  return _instance;
}

void
StateENVCONN::exec (EntityManager *manager)
{
  std::string tmp;

  if (!manager->probeStatus (&tmp))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("ENVCONN", "Alert", "going to state UNAV.");
      switchState (manager, StateUNAV::Instance ());
      return;
    }
  /// If entity manager is connected to the environment go to IDLE state
  if (manager->getConnected ())
    {
      manager->setConnectSignal (false);
      manager->stateLog ("ENVCONN", "Alert", "going to state IDLE.");
      switchState (manager, StateIDLE::Instance ());
    }
  /// If not signal the manager to attempt to connect
  else
    {
      manager->setConnectSignal (true);
    }
}
