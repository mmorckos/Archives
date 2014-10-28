/*
 * stateBUSY.h
 *
 *  Created on: 2012-02-09
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATEBUSY_H_
#define STATEBUSY_H_

#include <iostream>
#include <vector>

#include "state.h"

class EntityManager;

class StateBUSY : public State
{

public:
  static StateBUSY* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateBUSY *_instance;
  std::string taskStatus;

protected:
  StateBUSY ();

  int getTaskFdbk (EntityManager *manager, std::vector <std::string> *replyList);
};

#endif /* STATEBUSY_H_ */
