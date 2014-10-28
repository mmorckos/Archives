/*
 * entityserver.h
 *
 *  Created on: 2011-12-04
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef ENTITYSERVER_H_
#define ENTITYSERVER_H_

#include <Ice/Ice.h>
#include <Ice/Service.h>
#include <IceUtil/Timer.h>

#include "sessionmanager.h"

class EntityServer : public Ice::Service
{

public:
  EntityServer ();
  virtual ~EntityServer ();

  bool init ();
  virtual bool start (int argc, char *argv[], int& status);
  virtual bool stop ();

protected:
  Ice::ObjectAdapterPtr adapter_Ptr;
  IceUtil::TimerPtr timer_Ptr;
};

#endif /* ENTITYSERVER_H_ */
