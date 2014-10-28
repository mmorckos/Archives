/*
 * entityinterfacer.cpp
 *
 *  Created on: 2012-01-30
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "entityinterfacer.h"
#include "./util/configparser.h"
#include "./util/dataformatconverter.h"
#if defined (__linux__)
#include "./common/status.h"
#elif defined (_WIN32)
#include "..\\common\\common.h"
#endif
#include "./protobufdata/param.pb.h"
#include "./protobufdata/ctrlparam.pb.h"
#include "./protobufdata/resource.pb.h"
#include "./protobufdata/task.pb.h"

EntityInterfacer::EntityInterfacer () :
  status (IDLE),
  regsitered (false),
  basicPrflUrl (""),
  svcPrflUrl (""),
  communicator_Ptr (0),
  timer_Ptr (0),
  readiness (0),
  connected (false),
  running (false),
  execTaskStatus (ERR),
  execTaskFdbk (""),
  callee (0),
  taskCallback_Ptr (0),
  intParamCallback_Ptr (0),
  doubleParamCallback_Ptr (0),
  stringParamCallback_Ptr (0),
  booleanParamCallback_Ptr (0),
  resourceCallback_Ptr (0),
  updateCallback_Ptr (0),
  stopCallback_Ptr (0)
{
  for (unsigned int i = 0; i < 3; ++i)
      currPos.push_back (-1.0);

  posData[0] = '\0';
}

EntityInterfacer::~EntityInterfacer ()
{}

bool
EntityInterfacer::init (const std::string& fileName, const bool flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  Util::ConfigParser parser;
  std::map <std::string, std::string> parsedData;
  std::map <std::string, std::string>::iterator itr;
  unsigned int porta0;
  unsigned int porta1;
  long sendTimeout;
  long recvTimeout;
  bool proto;
  unsigned int count;

  logFlag = flag;
  porta0 = 0;
  porta1 = 0;
  sendTimeout = 0;
  recvTimeout = 0;
  proto = 0;
  count = 0;
  readiness &= 511;

  if (logFlag)
      logger.init ("", false);

  if (!parser.setFile (fileName))
    {
      if (logFlag)
          log ("Error", "port configurations file could not be open. File does not exits.");

      return false;
    }

  if (!parser.parse (&parsedData))
    {
      if (logFlag)
          log ("Error", "port configurations file could not be open. File does not exits.");

      return false;
    }

  for (itr = parsedData.begin (); itr != parsedData.end (); ++itr)
    {
      if ((*itr).first == "PORTa0")
        {
          porta0 = atoi (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "PORTa1")
        {
          porta1 = atoi (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "SNDTMO")
        {
          sendTimeout = atol (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "RECVTMO")
        {
          recvTimeout = atol (((*itr).second).c_str ());
          ++count;
        }
      else if ((*itr).first == "PROTO")
        {
          proto = atoi (((*itr).second).c_str ());
          ++count;
        }
      else
        {
          if (logFlag)
              log ("Warning", "unknown configuration parameter.");
        }
    }

  if (count != CONFIG_PARAM_COUNT)
    {
      if (logFlag)
          log ("Error", "missing port configuration parameters.");

      return false;
    }
  /// Setting port numbers range
  if (!commSckt.setPortRange (porta0, porta1))
    {
      if (logFlag)
          log ("Error", "failed to set port numbers range. Invalid range.");

      return false;
    }
  /// Initializing socket
  if (commSckt.init (proto, recvTimeout, sendTimeout) == ERR)
    {
      if (logFlag)
          log ("Error", "failed to initialize sockets.");

      return false;
    }

  /// Initializing communicator
  communicator_Ptr = new Communicator (this, logFlag);
  readiness ^= 512;

  if (logFlag)
      log ("Info", "sockets initialization is success.");

  return true;
}

bool
EntityInterfacer::connect ()
{
  if (connected)
    {
      if (logFlag)
          log ("Warning", "Already connected to EntityManager.");

      return true;
    }

  if (readiness < READINESS_0)
    {
      if (logFlag)
          log ("Error", "Cannot proceed with connection. Initialization is not done.");

      return false;
    }
  else if (readiness >= READINESS_0 && readiness < MAXREADINESS)
    {
      if (logFlag)
          log ("Error", "Cannot proceed with connection. Not all callbacks are set.");

      return false;
    }

  if (logFlag)
      log ("Info", "Listening for EntityManager...");

  if (commSckt.openConn () != ERR)
    {
      connected = true;
      if (logFlag)
          log ("Info", "Connected to EntityManager.");

      return true;
    }

  if (logFlag)
      log ("Error", "failed to connect to EntityManager.");

  return false;
}

void
EntityInterfacer::disconnect ()
{
  if (!connected)
    {
      if (logFlag)
          log ("Warning", "Already disconnected from EntityManager.");

      return;
    }

  commSckt.closeConn ();
  connected = false;
  if (logFlag)
      log ("Info", "Disconnected from EntityManager.");
}

bool
EntityInterfacer::start ()
{
  IceUtil::Mutex::Lock sync (startMutex);

  if (running)
    {
      if (logFlag)
          log ("Warning", "Already running.");

      return true;
    }

  if (!connect ())
      return false;
  /// Clean-up
  if (timer_Ptr)
    {
      timer_Ptr->destroy ();
      timer_Ptr = 0;
    }

  timer_Ptr = new IceUtil::Timer();

  communicator_Ptr->reset ();
  timer_Ptr->scheduleRepeated (communicator_Ptr, IceUtil::Time::milliSeconds (50));
  running = true;
  if (logFlag)
      log ("Info", "Started up and running successfully.");

  return true;
}

void
EntityInterfacer::stop ()
{
  IceUtil::Mutex::Lock sync (stopMutex);

  if (!running)
    {
      if (logFlag)
          log ("Warning", "already stopped.");

      return;
    }
  /// Stop all running background threads
  timer_Ptr->cancel (communicator_Ptr);
  disconnect ();
  setRegistered (false);
  running = false;
  if (logFlag)
      log ("Info", "stopped.");
}


void
EntityInterfacer::uploadBasicProfile (const std::string& fileUrl)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  basicPrflUrl = "";
  basicPrflUrl.append (fileUrl);
}

void
EntityInterfacer::uploadServiceProfile (const std::string& fileUrl)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  svcPrflUrl = "";
  svcPrflUrl.append (fileUrl);
}

void
EntityInterfacer::setStatus (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  status = flag ? READY : BUSY;
}

std::string
EntityInterfacer::getStatus ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return status;
}

void
EntityInterfacer::setRegistered (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  regsitered = flag;
}

bool
EntityInterfacer::getRegistered ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return regsitered;
}

bool
EntityInterfacer::getBasicProfile (std::string *msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = basicPrflUrl.size () > 0;
  if (flag)
      (*msg) = basicPrflUrl;

  return flag;
}

bool
EntityInterfacer::getSvcProfile (std::string *msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = svcPrflUrl.size () > 0;
  if (flag)
      (*msg) = svcPrflUrl;

  return flag;
}

bool
EntityInterfacer::getPositionUpdate (char *data, size_t size)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = strlen (posData);
  if (flag)
    {
      for (size_t i = 0; i < BUFFSIZE; ++i)
          data[i] = posData[i];

      posData[0] = '\0';
    }

  return flag;
}

void
EntityInterfacer::updateHealth (const std::string& healthData)
{
/// TODO
/*
  IceUtil::Mutex::Lock sync (sendMutex);

  std::string msg;

  compileMsg (HLTH, healthData, &msg);
  addDataItem (msg);
*/
}

bool
EntityInterfacer::updatePosition (const double& xPos, const double& yPos, const double& zPos)
{
  IceUtil::Mutex::Lock sync (sendMutex);

  Data::Position position;
  DataFormatConverter ft;
  /// If there was a change
  if (currPos[0] != xPos || currPos[1] != yPos || currPos[2] != zPos)
    {
      currPos[0] = xPos;
      currPos[1] = yPos;
      currPos[2] = zPos;
      position.set_xpos (xPos);
      position.set_ypos (yPos);
      position.set_zpos (zPos);
      if (!ft.formatPosition (&position, posData, BUFFSIZE, true))
        {
          if (logFlag)
              log ("Error", "Invalid position data. Message will be discarded.");

          return false;
        }
    }

  return true;
}

void
EntityInterfacer::sendInfoMsg (const std::string& msgBody)
{
  IceUtil::Mutex::Lock sync (sendMutex);

  std::string msg;

  compileMsg (INFO, msgBody, &msg);
  addInfoMsg (msg);
}

bool
EntityInterfacer::sendTaskFeedback (const unsigned int& serviceId, const bool& taskStatus,
    const std::string& fdbkMsg)
{
  IceUtil::Mutex::Lock sync (sendMutex);

  if (!getRegistered ())
    {
      if (logFlag)
          log ("Warning", "entity is not registered to a client. Message will be discarded.");

      return false;
    }
  else if (getStatus () == READY)
    {
      if (logFlag)
          log ("Error", "entity does not appear to be at BUSY state. Message will be discarded.");

      return false;
    }
  execTaskStatus = taskStatus ==  true ? OK : FALSE;
  execTaskFdbk = fdbkMsg;

  return false;
}

bool
EntityInterfacer::sendData (const std::string& serviceId, const std::string& dataUrl)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
/*
  IceUtil::Mutex::Lock sync (sendMutex);

  std::string msg;

  if (!getRegistered ())
    {
      if (logFlag)
          log ("Warning", "entity is not registered to a client. Message is discarded.");

      return;
    }

  compileMsg (DATA, dataUrl, &msg);
  addDataItem (msg);
*/
}


bool
EntityInterfacer::sendIntParam (const unsigned int& serviceId, const unsigned int& id,
    const int& value)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
/*
  IceUtil::Mutex::Lock sync (sendMutex);

  DataFormatConverter ft;
  Data::Param param;
  std::string tmp;

  if (!getRegistered ())
    {
      if (logFlag)
          log ("Warning", "entity is not registered to a client. Message will be discarded.");

      return false;
    }

  param.set_serviceid (serviceId);
  param.set_id (id);
  param.set_ctrlparamid (id);
  param.set_val0 (value);
  param.set_datatype (Data::Param_DataType_INT);

  if (!ft.formatParam (&param, &tmp, true))
    {
      if (logFlag)
          log ("Error", "Invalid parameter. Message will be discarded.");

      return false;
    }

  addDataItem (tmp);

  return true;
*/
}

bool
EntityInterfacer::sendDoubleParam (const unsigned int& serviceId, const unsigned int& id,
    const double& value)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
/*
  IceUtil::Mutex::Lock sync (sendMutex);

  DataFormatConverter ft;
  Data::Param param;
  std::string tmp;

  if (!getRegistered ())
    {
      if (logFlag)
          log ("Warning", "entity is not registered to a client. Message is discarded.");

      return false;
    }

  param.set_serviceid (serviceId);
  param.set_id (id);
  param.set_ctrlparamid (id);
  param.set_val1 (value);
  param.set_datatype (Data::Param_DataType_DBL);

  if (!ft.formatParam (&param, &tmp, true))
    {
      if (logFlag)
          log ("Error", "Invalid parameter. Message will be discarded.");

      return false;
    }

  addDataItem (tmp);

  return true;
*/
}

bool
EntityInterfacer::sendStringParam (const unsigned int& serviceId, const unsigned int& id,
    const std::string& value)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
/*
  IceUtil::Mutex::Lock sync (sendMutex);

  DataFormatConverter ft;
  Data::Param param;
  std::string tmp;

  if (!getRegistered ())
    {
      if (logFlag)
          log ("Warning", "entity is not registered to a client. Message is discarded.");

      return false;
    }

  param.set_serviceid (serviceId);
  param.set_id (id);
  param.set_ctrlparamid (id);
  param.set_val2 (value);
  param.set_datatype (Data::Param_DataType_STR);

  if (!ft.formatParam (&param, &tmp, true))
    {
      if (logFlag)
          log ("Error", "Invalid parameter. Message will be discarded.");

      return false;
    }

  addDataItem (tmp);

  return true;
*/
}

bool
EntityInterfacer::sendBooleanParam (const unsigned int& serviceId, const unsigned int& id,
    const bool& value)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
/*
  IceUtil::Mutex::Lock sync (sendMutex);

  DataFormatConverter ft;
  Data::Param param;
  char tmp [BUFFIZE];

  if (!getRegistered ())
    {
      if (logFlag)
          log ("Warning", "entity is not registered to a client. Message will is discarded.");

      return false;
    }

  param.set_serviceid (serviceId);
  param.set_id (id);
  param.set_ctrlparamid (id);
  param.set_val3 (value);
  param.set_datatype (Data::Param_DataType_BOOL);

  if (!ft.formatParam (&param, tmp, BUFFSIZE, true))
    {
      if (logFlag)
          log ("Error", "Invalid parameter. Message will be discarded.");

      return false;
    }

  addDataItem (tmp);

  return true;
*/
}

bool
EntityInterfacer::sendIntListParam (const unsigned int& serviceId, const unsigned int& id,
    const std::vector <int>& list)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
}

bool
EntityInterfacer::sendDoubleListParam (const unsigned int& serviceId, const unsigned int& id,
      const std::vector <double>& list)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
}

bool
EntityInterfacer::sendStringListParam (const unsigned int& serviceId, const unsigned int& id,
      const std::vector <std::string>& list)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
}

bool
EntityInterfacer::sendBoolListParam (const unsigned int& serviceId, const unsigned int& id,
      const std::vector <bool>& list)
{
  IceUtil::Mutex::Lock sync (sendMutex);
  /// TODO
  if (logFlag)
      log ("Warning", "Not supported. Message will be discarded.");

  return false;
}

void
EntityInterfacer::addDataItem (const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  if (!connected)
    {
      if (logFlag)
          log ("Warning", "Unable to send message. Connection is not active.");

      return;
    }

  dataQueue.push (msg);
}

bool
EntityInterfacer::getDataItem (std::string *msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = dataQueue.size () > 0;
  if (flag)
    {
      *msg = dataQueue.front ();
      dataQueue.pop ();
    }

  return flag;
}

void
EntityInterfacer::addInfoMsg (const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  if (!connected)
    {
      if (logFlag)
          log ("Warning", "Unable to send message. Connection is not active.");

      return;
    }

  infoMsgQueue.push (msg);
}

bool
EntityInterfacer::getInfoMsg (std::string *msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = infoMsgQueue.size () > 0;
  if (flag)
    {
      (*msg) = infoMsgQueue.front ();
      infoMsgQueue.pop ();
    }

  return flag;
}

void
EntityInterfacer::getTaskFeedback (int *status, std::string *fdbk)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  (*status) = execTaskStatus;
  (*fdbk) = execTaskFdbk;
  execTaskStatus = ERR;
  execTaskFdbk = "";
}

bool
EntityInterfacer::setCallee (void *c)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 767;
  if (c == NULL)
    {
      if (logFlag)
        log ("Error", "pointer to callee is undefined.");

      return false;
    }

  callee = c;
  readiness ^= 256;

  return true;
}

void
EntityInterfacer::setTaskCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 895;
  taskCallback_Ptr = ptrFunc;
  readiness ^= 128;
}

void
EntityInterfacer::setIntParamCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
    const unsigned int& id, const int& value))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 959;
  intParamCallback_Ptr = ptrFunc;
  readiness ^= 64;
}

void
EntityInterfacer::setIntParamListCallback (bool (*ptrFunc) (void *callee,
    const unsigned int& serviceId, const unsigned int& id, const std::vector <int>& values))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  ///TODO
}

void
EntityInterfacer::setDoubleParamCallback (bool (*ptrFunc) (void *callee,
    const unsigned int& serviceId, const unsigned int& id, const double& value))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 991;
  doubleParamCallback_Ptr = ptrFunc;
  readiness ^= 32;
}

void
EntityInterfacer::setDoubleParamListCallback (bool (*ptrFunc) (void *callee,
    const unsigned int& serviceId, const unsigned int& id, const std::vector <double>& values))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  ///TODO
}

void
EntityInterfacer::setStringParamCallback (bool (*ptrFunc) (void *callee,
    const unsigned int& serviceId, const unsigned int& id, const std::string& value))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 1007;
  stringParamCallback_Ptr = ptrFunc;
  readiness ^= 16;
}

void
EntityInterfacer::setStringParamListCallback (bool (*ptrFunc) (void *callee,
    const unsigned int& serviceId, const unsigned int& id, const std::vector <std::string>& values))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  ///TODO
}

void
EntityInterfacer::setBooleanParamCallback (bool (*ptrFunc) (void *callee,
    const unsigned int& serviceId, const unsigned int& id, const bool& value))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 1015;
  booleanParamCallback_Ptr = ptrFunc;
  readiness ^= 8;
}

void
EntityInterfacer::setBooleanParamListCallback (bool (*ptrFunc) (void *callee,
    const unsigned int& serviceId, const unsigned int& id, const std::vector <bool>& values))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  ///TODO
}

void
EntityInterfacer::setResourceCallback (bool (*ptrFunc) (void *callee, const unsigned int& serviceId,
    const unsigned int& resId, const std::string& url))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 1019;
  resourceCallback_Ptr = ptrFunc;
  readiness ^= 4;
}

void
EntityInterfacer::setUpdateCallback (bool (*ptrFunc) (void *callee, const std::string& msg))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 1021;
  updateCallback_Ptr = ptrFunc;
  readiness ^= 2;
}

void
EntityInterfacer::setStopCallback (bool (*ptrFunc) (void *callee, const std::string& msg))
{
  IceUtil::Mutex::Lock sync (dataMutex);

  readiness &= 1022;
  stopCallback_Ptr = ptrFunc;
  readiness ^= 1;
}

void
EntityInterfacer::testCallbacks ()
{
  std::cout << "Test: " << taskCallback_Ptr (callee, 2) << std::endl;
  std::cout << "PRINTS: " << intParamCallback_Ptr (callee, 2, 3, 3) << std::endl;
  std::cout << "PRINTS: " << doubleParamCallback_Ptr (callee, 2, 3, 3) << std::endl;
  std::cout << "PRINTS: " << stringParamCallback_Ptr (callee, 2, 3, "asdasd") << std::endl;
  std::cout << "PRINTS: " << resourceCallback_Ptr (callee, 2, 3, "asdasd") << std::endl;
  std::cout << "PRINTS: " << updateCallback_Ptr(callee, "asdasd") << std::endl;
  std::cout << "PRINTS: " << stopCallback_Ptr (callee, "sd") << std::endl;
}

void
EntityInterfacer::emergencyStop (const std::string& statusMsg)
{
  IceUtil::Mutex::Lock sync (emergencyStopMutex);

  stopCallback_Ptr (callee, statusMsg);
  if (logFlag)
      log ("Alert", "sent emergency stop signal.");
}

void
EntityInterfacer::compileMsg (const std::string& prefix, const std::string& msgBody,
    std::string *msg)
{
  msg->append (prefix);
  msg->append (DELIM);
  msg->append (msgBody);
}

void
EntityInterfacer::log (const std::string& type, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (logMutex);

  logger.logMsg (type, ENTITY_INTERFACER, msg);
}
