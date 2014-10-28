/*
 * stateINIT.h
 *
 *  Created on: 2011-11-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATEINIT_H_
#define STATEINIT_H_

#include "state.h"

class EntityManager;

class StateINIT : public State
{

public:
  static StateINIT* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateINIT *_instance;
  bool loadedBasicPrfl;
  bool loadedSvcPrfl;

  inline void reset ();

protected:
  StateINIT ();
};

#endif /* STATEINIT_H_ */
