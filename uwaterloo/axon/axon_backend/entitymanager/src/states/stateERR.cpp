/*
 * stateERR.cpp
 *
 *  Created on: 2012-01-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <iostream>

#include "stateERR.h"
#include "stateINIT.h"
#include "../entitymanager.h"
#include "../common/global.h"
#include "../common/status.h"

StateERR *StateERR::_instance = 0;

/*! \brief Constructor.
 */
StateERR::StateERR ()
{}

StateERR*
StateERR::Instance ()
{
  if (_instance == 0)
      _instance = new StateERR;

  return _instance;
}

void
StateERR::exec (EntityManager *manager)
{
  std::string input;

  while (true)
    {
      manager->stateLog ("ERR", "Input", "*** Enter (R) to reset or (E) to exit:");
      std::cin >> input;
      if (input == "E" || input == "e")
        {
          manager->stateLog ("ERR", "Alert", "exiting...");
          exit (0);
        }
      else if (input == "R" || input == "r")
        {
          manager->stateLog ("ERR", "Alert", "going to state INIT.");
          switchState (manager, StateINIT::Instance ());
          return;
        }
      else
        {
          manager->stateLog ("ENVCONN", "Error", "unknown option.");
        }
    }
}
