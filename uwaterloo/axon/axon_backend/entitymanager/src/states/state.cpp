/*
 * state.cpp
 *
 *  Created on: 2012-01-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "state.h"
#include "../entitymanager.h"

void State::switchState (EntityManager *manager, State *nextState)
{
  manager->switchState (nextState);
}
