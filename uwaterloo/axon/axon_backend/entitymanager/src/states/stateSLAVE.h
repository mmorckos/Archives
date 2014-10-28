/*
 * stateSLAVE.h
 *
 *  Created on: 2012-04-22
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATESLAVE_H_
#define STATESLAVE_H_

#include "state.h"

class EntityManager;

class StateSLAVE : public State
{

public:
  static StateSLAVE* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateSLAVE *_instance;
  bool entry;

protected:
  StateSLAVE ();
};

#endif /* STATESLAVE_H_ */
