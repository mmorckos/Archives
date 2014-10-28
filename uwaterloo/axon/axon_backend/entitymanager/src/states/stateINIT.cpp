/*
 * stateINIT.cpp
 *
 *  Created on: 2011-11-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateINIT.h"
#include "stateENVCONN.h"
#include "stateUNAV.h"
#include "stateERR.h"
#include "../entitymanager.h"
#include "../common/global.h"
#include "../common/status.h"

StateINIT *StateINIT::_instance = 0;

/*! \brief Constructor.
 */
StateINIT::StateINIT () :
  loadedBasicPrfl (false),
  loadedSvcPrfl (false)
{}

StateINIT*
StateINIT::Instance ()
{
  if (_instance == 0)
      _instance = new StateINIT;

  return _instance;
}

void
StateINIT::exec (EntityManager *manager)
{
  bool status;
  std::string reply;
  std::string tmp;

  if (!manager->getEntityOnline () || !manager->probeStatus (&tmp))
    {
      manager->setEntityOnline (false);
      manager->stateLog ("INIT", "Alert", "going to state UNAV.");
      switchState (manager, StateUNAV::Instance ());
      return;
    }

  if (!loadedSvcPrfl)
    {
      manager->sendEntitySyncMsg (SVCPRFL, " ", &reply);
      /// If file url acquired load the service profile
      if (reply != NONE)
        {
          if (!manager->loadServicesProfile (reply) || !manager->serviceProfiler.verify ())
            {
              reset ();
              manager->stateLog ("INIT", "Alert", "going to state ERR.");
              switchState (manager, StateERR::Instance ());
              return;
            }
          else
            {
              loadedSvcPrfl = true;
            }
        }
      /// If could not acquire the service profile try again
      else
        {
          return;
        }
    }

  reply = "";
  if (!loadedBasicPrfl)
    {
      /// Attempting to acquire the basic profile file url
      manager->sendEntitySyncMsg (PRFL, " ", &reply);
      /// If file URL acquired load the basic profile
      if (reply != NONE)
        {
          if (!manager->loadBasicPrfl (reply))
            {
              reset ();
              manager->stateLog ("INIT", "Alert", "going to state ERR.");
              switchState (manager, StateERR::Instance ());
              return;
            }
          else
            {
              loadedBasicPrfl = true;
            }
        }
      /// If could not acquire the basic profile try again
      else
        {
          return;
        }
    }
  /// If all passed switch to ENVCONN state
  reset ();
  manager->stateLog ("INIT", "Alert", "going to state ENVCONN.");
  switchState (manager, StateENVCONN::Instance ());
}

void
StateINIT::reset ()
{
  loadedBasicPrfl = false;
  loadedSvcPrfl = false;
}
