/*
 * stateUNAV.cpp
 *
 *  Created on: 2011-11-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "stateUNAV.h"
#include "stateINIT.h"
#include "../entitymanager.h"
#include "../common/global.h"
#include "../common/status.h"

StateUNAV *StateUNAV::_instance = 0;

/*! \brief Constructor.
 */
StateUNAV::StateUNAV () :
    entry (true)
{}

StateUNAV*
StateUNAV::Instance ()
{
  if (_instance == 0)
      _instance = new StateUNAV;

  return _instance;
}

/*! \brief This function houses the main logic of the state.
 *
 * The UNAV state will attempt to start up entity's process AND/OR establish connection with the
 * entity. If successful, it will shift to the IDLE state.
 */
void
StateUNAV::exec (EntityManager *manager)
{
  std::string reply;
  bool flag;
  /// Reset the entity manager in preparation of a new connection attempt
  if (entry)
    {
      manager->resetAll ();
      entry = false;
    }

  if (!isEntityProcessRunning ())
    {
      flag = startUpEntity ();
    }

  sleep (1);
  /// If connection to entity is success
  if (flag && manager->openEntityConn ())
    {
      /// If is entity connected to is authentic proceed to the INIT state
      if (manager->sendEntitySyncMsg (ENTITY_MANAGER , " ", &reply) && reply == ENTITY)
        {
          manager->setEntityOnline (true);
          entry = true;
          manager->stateLog ("UNAV", "Alert", "going to state INIT.");
          switchState (manager, StateINIT::Instance ());
        }
      /// Entity is not responsive or could not be authenticated
      else
        {
          manager->closeEntityConn ();
        }
    }
  /// Close the connection and try again
  else
    {
      manager->closeEntityConn ();
    }
}

/*! \brief This function checks if the entity's process is running.
 *
 * \return true if entity's process is running. false otherwise.
 */
bool
StateUNAV::isEntityProcessRunning ()
{
  /// TODO
  return false;
}

/*! \brief This function starts the entity's process.
 *
 * \return true if entity's process was successfully run. false otherwise.
 */
bool
StateUNAV::startUpEntity ()
{
  /// TODO
  return true;
}
