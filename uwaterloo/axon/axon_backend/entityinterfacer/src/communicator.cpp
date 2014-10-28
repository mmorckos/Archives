/*
 * communicator.cpp
 *
 *  Created on: 2012-02-06
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "communicator.h"
#include "entityinterfacer.h"
#include "./protobufdata/param.pb.h"
#include "./protobufdata/resource.pb.h"
#include "./common/global.h"
#include "./common/status.h"
#include "./util/dataformatconverter.h"

Communicator::Communicator (EntityInterfacer *ei, const bool flag) :
  entityInterfacer (ei),
  logFlag (flag),
  running (true)
{}

Communicator::~Communicator ()
{}

void
Communicator::runTimerTask ()
{
  std::string msg;
  std::string prefix;
  std::string body;

  if (running)
    {
      /// If expecting a data item
      if (getExpectedDataType ().size () > 0)
        {
          char msg [BUFFSIZE];

          if (entityInterfacer->commSckt.getMsg (msg, BUFFSIZE) != OK)
            {
              entityInterfacer->emergencyStop ("Lost connection.");
              running = false;
            }
          else
            {
              processData (msg, BUFFSIZE);
              setExpectedDataType ("");
            }
        }
      /// If it is a normal message
      else
        {
          msg = "";
          /// If connection timed out or failed the entity has to be informed instantly
          if (entityInterfacer->commSckt.getMsg (&msg) != OK)
            {
              entityInterfacer->emergencyStop ("Lost connection.");
              running = false;
            }
          /// Relay the message to the EntityInterfacer which will relay it to the entity itself
          else
            {
              if (!extractMsg (msg, &prefix, &body))
                  sendMsg (BAD_MSG);
              else
                  processMsg (msg, prefix, body);
            }
        }
    }
}

void
Communicator::reset ()
{
  running = true;
}

bool
Communicator::extractMsg (const std::string& msg, std::string *prefix, std::string *body)
{
  size_t pos;

  pos = msg.find (DELIM);

  if (pos == std::string::npos)
    {
      if (logFlag)
          entityInterfacer->log ("Error", "Message is not properly formatted.");

      return false;
    }

  (*prefix) = msg.substr (0, pos);
  (*body) = msg.substr (pos + 1);

  if (prefix->size () == 0 || body->size () == 0)
    {
      if (logFlag)
          entityInterfacer->log ("Error", "Message body is missing.");

      return false;
    }

  return true;
}

void
Communicator::processMsg (std::string msg, const std::string& prefix, const std::string& body)
{
  std::string tmp;
  int status;
  char reply [BUFFSIZE];
  DataFormatConverter ft;

  reply[0] = '\0';
  /// If it is an identification message
  if (prefix == ENTITY_MANAGER)
    {
      sendMsg (ENTITY);
    }
  /// If it is a probe for the current status
  else if (prefix == STAT)
    {
      sendMsg (entityInterfacer->getStatus ());
    }
  /// If it is a probe for the current position
  else if (prefix == POS)
    {
      if (entityInterfacer->getPositionUpdate (reply, BUFFSIZE))
          sendMsg (reply, BUFFSIZE);
      else
          sendMsg (NONE.c_str (), BUFFSIZE);
    }
  /// If it is a request for the basic profile file url
  else if (prefix == PRFL)
    {
      if (entityInterfacer->getBasicProfile (&tmp))
          sendMsg (tmp);
      else
          sendMsg (NONE);
    }
  /// If it is a request for the service profile file url
  else if (prefix == SVCPRFL)
    {
      if (entityInterfacer->getSvcProfile (&tmp))
          sendMsg (tmp);
      else
          sendMsg (NONE);
    }
  /// If it is a signal of incoming data item keep track of the type
  else if (prefix == DATA)
    {
      expectedDataType = body;
      sendMsg (OK_MSG);
    }
  /// If is a probe for the recently completed task
  else if (prefix == TSK_FDBK)
    {
      entityInterfacer->getTaskFeedback (&status, &tmp);

      if (status == OK)
          executingTsk.set_status (Data::Task_Status_SUCCESS);
      else if (status == FALSE)
          executingTsk.set_status (Data::Task_Status_FAILURE);
      else
          executingTsk.set_status (Data::Task_Status_STOPPED);

      executingTsk.set_fdbkmsg (tmp);
      ft.formatTask (&executingTsk, reply, BUFFSIZE, true);

      executingTsk.Clear ();
      sendMsg (reply, BUFFSIZE);
    }
  /// If it is a probe for pending client messages
  else if (prefix == CLIENT_INFOMSG)
    {
      if (entityInterfacer->getInfoMsg (&tmp))
          sendMsg (tmp);
      else
          sendMsg (NONE);
    }
  /// If it is a registration message
  else if (prefix == CHECKOUT_ENTITY)
    {
      sendMsg (OK_CHECKOUT);
      entityInterfacer->setRegistered (true);
      entityInterfacer->updateCallback_Ptr (entityInterfacer->callee, body);
    }
  /// If message is a unregister message
  else if (prefix == RELEASE_ENTITY)
    {
      sendMsg (OK_RELEASE);
      entityInterfacer->setRegistered (false);
      entityInterfacer->stopCallback_Ptr (entityInterfacer->callee, body);
    }
  /// If message is a stop signal
  else if (prefix == STOP)
    {
      sendMsg (OK_STOPPED);
      entityInterfacer->stopCallback_Ptr (entityInterfacer->callee, body);
    }
  /// If message is not recognized
  else
    {
      sendMsg (UNKNOWN_MSG);
    }
}

void
Communicator::processData (char *msg, const unsigned int size)
{
  unsigned int serviceId;
  unsigned int id;
  DataFormatConverter ft;
  char reply [BUFFSIZE];
  /// If message is a param
  if (expectedDataType == PARAM)
    {
      Data::Param param;
      /// If content could not be extracted
      if (!ft.formatParam (&param, msg, size, false))
        {
          ft.formatParam (&param, reply, BUFFSIZE, true, BAD_PARAM);
          sendMsg (reply, BUFFSIZE);
        }
      else
        {
          serviceId = param.serviceid ();
          id = param.id ();

          switch (param.datatype ())
          {
          case Data::Param_DataType_INT:
            {
              if (!entityInterfacer->intParamCallback_Ptr (entityInterfacer->callee, serviceId, id,
                  param.val0 ()))
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, DENY_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              else
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, OK_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              break;
            }
          case Data::Param_DataType_DBL:
            {
              if (!entityInterfacer->doubleParamCallback_Ptr (entityInterfacer->callee, serviceId,
                  id, param.val1 ()))
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, DENY_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              else
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, OK_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              break;
            }
          case Data::Param_DataType_STR:
            {
              if (!entityInterfacer->stringParamCallback_Ptr (entityInterfacer->callee, serviceId,
                  id, param.val2 ()))
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, DENY_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              else
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, OK_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              break;
            }
          case Data::Param_DataType_BOOL:
            {
              if (!entityInterfacer->booleanParamCallback_Ptr (entityInterfacer->callee, serviceId,
                  id, param.val3 ()))
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, DENY_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              else
                {
                  ft.formatParam (&param, reply, BUFFSIZE, true, OK_PARAM);
                  sendMsg (reply, BUFFSIZE);
                }
              break;
            }
          }
        }

      std::cout << std::endl;
    }
  /// If message is a resource
  else if (expectedDataType == RESOURCE)
    {
      Data::Resource res;

      if (!ft.formatResource (&res, msg, size, false))
        {
          ft.formatResource (&res, reply, BUFFSIZE, true, BAD_RES);
          sendMsg (reply, BUFFSIZE);
        }
      else
        {
          serviceId = res.serviceid ();
          id = res.id ();

          if (!entityInterfacer->resourceCallback_Ptr (entityInterfacer->callee, serviceId,
              id, res.url ()))
            {
              ft.formatResource (&res, reply, BUFFSIZE, true, DENY_RES);
              sendMsg (reply, BUFFSIZE);
            }
          else
            {
              ft.formatResource (&res, reply, BUFFSIZE, true, OK_RES);
              sendMsg (reply, BUFFSIZE);
            }
        }
    }
  /// If message is a task
  else if (expectedDataType == TSK)
    {
      Data::Task tsk;

      if (!ft.formatTask (&tsk, msg, size, false))
        {
          ft.formatTask (&tsk, reply, BUFFSIZE, true, BAD_TSK);
          sendMsg (reply, BUFFSIZE);
        }
      else
        {
          entityInterfacer->setStatus (false);

          if (!entityInterfacer->taskCallback_Ptr (entityInterfacer->callee, tsk.serviceid ()))
            {
              entityInterfacer->setStatus (true);
              ft.formatTask (&tsk, reply, BUFFSIZE, true, DENY_TSK);
              sendMsg (reply, BUFFSIZE);
            }
          /// If the entity accepted the task, status is set to BUSY and is sent to EntityManager
          else
            {
              /// Keeping track of the task to run
              tsk.set_status (Data::Task_Status_RUNNING);
              executingTsk.CopyFrom (tsk);
              ft.formatTask (&tsk, reply, BUFFSIZE, true, OK_TSK);
              sendMsg (reply, BUFFSIZE);
            }
        }
    }
}

void
Communicator::setExpectedDataType (std::string dataType)
{
  expectedDataType = dataType;
}

std::string
Communicator::getExpectedDataType ()
{
  return expectedDataType;
}

void
Communicator::sendMsg (const std::string& prefix, const std::string& body)
{
  std::string msg;

  msg.append (prefix);
  if (body.size () > 0)
    {
      msg.append (DELIM);
      msg.append (body);
    }

  if (entityInterfacer->commSckt.sendMsg (msg) != OK)
    {
      running = false;
      entityInterfacer->emergencyStop ("Lost connection to EntityManager.");
      entityInterfacer->stop ();
    }
}

void
Communicator::sendMsg (const char *msg, const unsigned int size)
{
  if (entityInterfacer->commSckt.sendMsg (msg, size) != OK)
    {
      running = false;
      entityInterfacer->emergencyStop ("Lost connection to EntityManager.");
      entityInterfacer->stop ();
    }
}
