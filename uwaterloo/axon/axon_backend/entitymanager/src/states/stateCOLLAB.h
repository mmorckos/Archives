/*
 * stateCOLLAB.h
 *
 *  Created on: 2012-04-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef STATECOLLAB_H_
#define STATECOLLAB_H_

#include <iostream>
#include <vector>
#include <map>

#include "state.h"
#include "../util/datarecord.h"

class EntityManager;

class StateCOLLAB : public State
{

public:
  static StateCOLLAB* Instance ();
  virtual void exec (EntityManager *manager);

private:
  static StateCOLLAB *_instance;
  Util::DataRecord collabTaskData;
  std::vector <std::string> collaborators;
  std::map <std::string, int> fulfillmentMap;
  std::string currPreced;
  int currFulfillment;
  unsigned int entityCount;
  bool entry;
  bool ready;

protected:
  StateCOLLAB ();

  void execTasks (EntityManager *manager);
  bool monitorProgress (EntityManager *manager);
  void reset ();
};

#endif /* STATECOLLAB_H_ */
