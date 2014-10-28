/*
 * stateUNAV.h
 *
 *  Created on: 2011-11-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATEUNAV_H_
#define STATEUNAV_H_

#include "state.h"

class EntityManager;

class StateUNAV : public State
{

public:
  static StateUNAV* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateUNAV *_instance;
  bool entry;

protected:
  StateUNAV ();

  bool isEntityProcessRunning ();
  bool startUpEntity ();
};

#endif /* STATEUNAV_H_ */
