/*
 * stateREADY.h
 *
 *  Created on: 2011-11-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATEIDLE_H_
#define STATEIDLE_H_

#include "state.h"

class EntityManager;

class StateIDLE : public State
{

public:
  static StateIDLE* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateIDLE *_instance;

protected:
  StateIDLE ();
};

#endif /* STATEIDLE_H_ */
