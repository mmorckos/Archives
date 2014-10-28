/*
 * stateENVCONN.h
 *
 *  Created on: 2012-01-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATEENVCONN_H_
#define STATEENVCONN_H_

#include "state.h"

class EntityManager;

class StateENVCONN : public State
{

public:
  static StateENVCONN* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateENVCONN *_instance;

protected:
  StateENVCONN ();
};

#endif /* STATEENVCONN_H_ */
