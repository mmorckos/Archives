/*
 * state.h
 *
 *  Created on: 2011-11-14
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATE_H_
#define STATE_H_

#include <assert.h>

class EntityManager;

class State
{

public:
  virtual ~State ()
  {}

  virtual void exec (EntityManager *manager) = 0;

protected:
  /*! \brief Constructor.
   */
  State ()
  {}

  void switchState (EntityManager *manager, State *nextState);
};

#endif /* STATE_H_ */
