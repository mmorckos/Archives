/*
 * stateERR.h
 *
 *  Created on: 2012-01-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATEERR_H_
#define STATEERR_H_

#include "state.h"

class EntityManager;

class StateERR : public State
{

public:
  static StateERR* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateERR *_instance;

protected:
  StateERR ();
};

#endif /* STATEERR_H_ */
