/*
 * stateREG.h
 *
 *  Created on: 2012-02-09
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATEREG_H_
#define STATEREG_H_

#include <iostream>
#include <vector>

#include "state.h"

class EntityManager;

class StateREG : public State
{

public:
  static StateREG* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateREG *_instance;
  bool taskFlag;

protected:
  StateREG ();

  bool extractMsg (const std::string& msg, std::string *prefix, std::string *body);
  void reset ();
};

#endif /* STATEREG_H_ */
