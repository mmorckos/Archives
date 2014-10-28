/*
 * frontendserver.h
 *
 *  Created on: 2011-12-21
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef FRONTENDSERVER_H_
#define FRONTENDSERVER_H_

#include <Ice/Ice.h>
#include <Ice/Service.h>
#include <IceUtil/Timer.h>

#include "sessionmanager.h"

class FrontEndServer : public Ice::Service
{

public:
  FrontEndServer ();
  virtual ~FrontEndServer ();

  bool init ();
  virtual bool start (int argc, char *argv[], int& status);
  virtual bool stop ();

protected:
  Ice::ObjectAdapterPtr adapter_Ptr;
  IceUtil::TimerPtr timer_Ptr;
};

#endif /* FRONTENDSERVER_H_ */
