/*
 * entitymanager.cpp
 *
 *  Created on: 2011-12-03
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "entitymanager.h"
#include "./states/state.h"
#include "./states/stateUNAV.h"

#if defined (__linux__)
#include "./common/global.h"
#include "./common/status.h"
#include "util/xmlprofileparser.h"
#include "./util/configparser.h"
#include "util/dataformatconverter.h"
#elif defined (_WIN32)
#include "..\\common\\common.h"
#include "..\\common\\entities.h"
#include "..\\util\\configparser.h"
#endif

EntityManager::EntityManager (const Ice::LoggerPtr& logger, const bool& flag) :
  backendInterfacer (logger, flag),
  serviceProfiler (logger, flag),
  PORTS_CFG ("../config/ports.cfg"),
  GENERAL_CFG ("../config/general.cfg"),
  PROFILE_CFG_COUNT (3),
  GENERAL_CFG_COUNT (3),
  FIELD_NAME_CC (0),
  FIELD_TYPE_CC (0),
  FIELD_DESCR_CC (0),
  svcPrflCount (0),
  counter (0),
  entityOnline (false),
  connected (false),
  connectSignal (false),
  checkedOut (false),
  requested (false),
  released (false),
  stop (false),
  stopMsg (""),
  errState (false),
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

EntityManager::~EntityManager ()
{}

bool
EntityManager::init (std::string uuid)
{
  Util::ConfigParser parser;
  std::map <std::string, std::string> parsedData;
  std::map <std::string, std::string>::iterator itr;
  unsigned int count;
  std::ostringstream oss;

  count = 0;
  /// Parsing general configurations
  if (!parser.setFile (GENERAL_CFG) || !parser.parse (&parsedData))
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityManager: " << "failed to load general configurations.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }
  else
    {
      for (itr = parsedData.begin (); itr != parsedData.end (); ++itr)
        {
          if ((*itr).first == "NAMECC")
            {
              FIELD_NAME_CC = atoi ((*itr).second.c_str ());
              ++count;
            }
          else if ((*itr).first == "TYPECC")
            {
              FIELD_TYPE_CC = atoi ((*itr).second.c_str ());
              ++count;
            }
          else if ((*itr).first == "DESCRCC")
            {
              FIELD_DESCR_CC = atoi ((*itr).second.c_str ());
              ++count;
            }
          else
            {
              if (logFlag)
                {
                  oss.str ("");
                  oss << "EntityManager: " << "unknown parameters in general configurations.";
                  iceLogger_Ptr->trace ("Warning", oss.str ());
                }
            }
        }

      if (count != GENERAL_CFG_COUNT)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "EntityManager: ";
              oss << "missing or extraneous parameters in general configurations.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
    }

  if (!serviceProfiler.init ())
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityManager: " << "failed to initialize service profiler.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

  if (!backendInterfacer.init (PORTS_CFG))
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityManager: " << "failed to initialize back-end interfacer.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }
  entityUuid = uuid;
  currState = StateUNAV::Instance ();

  return true;
}

void
EntityManager::switchState (State *newState)
{
  currState = newState;
}

void
EntityManager::exec ()
{
  currState->exec (this);
}

bool
EntityManager::openEntityConn ()
{
  if (!backendInterfacer.openConn ())
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "EntityManager: " << "entity not found.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

  return true;
}

void
EntityManager::resetEntityConn ()
{
  backendInterfacer.closeConn ();
}

bool
EntityManager::probeStatus (std::string *status)
{
  return sendEntitySyncMsg (STAT, " ", status);
}

void
EntityManager::setEntityOnline (const bool& flag)
{
  IceUtil::Mutex::Lock sync (connMutex0);

  entityOnline = flag;
}

bool
EntityManager::getEntityOnline ()
{
  IceUtil::Mutex::Lock sync (connMutex0);

  return entityOnline;
}

void
EntityManager::setConnected (const bool& flag)
{
  IceUtil::Mutex::Lock sync (connMutex1);

  connected = flag;
}

bool
EntityManager::getConnected ()
{
  IceUtil::Mutex::Lock sync (connMutex1);

  return connected;
}

void
EntityManager::setConnectSignal (const bool& flag)
{
  IceUtil::Mutex::Lock sync (connMutex1);

  connectSignal = flag;
}

bool
EntityManager::getConnectSignal ()
{
  IceUtil::Mutex::Lock sync (connMutex1);

  return connectSignal;
}

void
EntityManager::setRequested (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  requested = flag;
}

bool
EntityManager::getRequested ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return requested;
}

void
EntityManager::setReleased (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  released = flag;
}

bool
EntityManager::getReleased ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return released;
}

void
EntityManager::setCheckedOut (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  checkedOut = flag;
}

bool
EntityManager::getCheckedOut ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return checkedOut;
}

void
EntityManager::toggleCollaborationMode (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  collaboration = flag;
}

bool
EntityManager::collaborationMode ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return collaboration;
}

void
EntityManager::toggleSlaveMode (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  slave = flag;
}

bool
EntityManager::slaveMode ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return slave;
}

void
EntityManager::toggleStop (const bool& flag, const std::string& msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  stop = flag;
  stopMsg = msg;
}

bool
EntityManager::stopSignal (std::string *msg)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  (*msg) = stopMsg;

  return stop;
}

///FIXME Temporary fix
void
EntityManager::setInitiator (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  initiator = flag;
}
///FIXME Temporary fix
bool
EntityManager::getInitiator ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return initiator;
}

void
EntityManager::setClientUuid (const std::string& uuid)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  clientUuid = "";
  clientUuid.append (uuid);
}

const std::string&
EntityManager::getClientUuid ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return clientUuid;
}

const std::string&
EntityManager::getUuid ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return entityUuid;
}

void
EntityManager::setCollaboratorUuid (const std::string& uuid)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  collaboratorEntityUuid = uuid;
}

const std::string&
EntityManager::getCollaboratorUuid ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return collaboratorEntityUuid;
}

bool
EntityManager::loadBasicPrfl (const std::string& fileUrl)
{
  XmlProfileParser parser;
  DataFormatConverter ft;
  unsigned int count;
  std::ostringstream oss;

  parser.setFile (fileUrl);

  if (!parser.parse (&basicPrfl))
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityManager: " << "failed to parse profile: " << parser.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

  basicPrfl.set_uuid (entityUuid);
  /// Adding service names (From service profile)
  count = serviceProfiler.getNumOfSvc ();
  for (unsigned int i = 0; i < count; ++i)
      basicPrfl.add_servicenamelist (serviceProfiler.getServiceName (i));

  if (!ft.formatBasicProfile (&basicPrfl, &basicPrflData, true))
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityManager: " << "failed to format basic profile: " << ft.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

  if (logFlag)
    {
      oss.str ("");
      oss << "EntityManager: " << "loaded basic profile.";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }

  return true;
}

void
EntityManager::getBasisPrflData (std::string *data)
{
  *data = basicPrflData;
}

bool
EntityManager::loadServicesProfile (const std::string& fileUrl)
{
  bool flag;
  std::ostringstream oss;

  serviceProfiler.clear ();
  flag = serviceProfiler.load (fileUrl);

  if (!flag)
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "EntityManager: " << "failed to load services profile.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

  if (logFlag)
    {
      oss.str ("");
      oss << "EntityManager: " << "loaded services profile.";
      iceLogger_Ptr->trace ("Info", oss.str ());
    }

  return true;
}

void
EntityManager::compileSvcProfile ()
{
  unsigned int size;

  size = serviceProfiler.getNumOfSvc ();
  setSvcPrflCount (size);

  for (unsigned i = 0; i < size; ++i)
    {
      Util::DataRecord svcDataRecord;

      if (serviceProfiler.compileServiceRecord (i, &svcDataRecord))
        {
          addSvcDataRecord (svcDataRecord);
        }
      else
        {
          setErrState (true);
          return;
        }
    }
}

void
EntityManager::setSvcPrflCount (unsigned int count)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  svcPrflCount = count;
}

unsigned int
EntityManager::getSvcPrflCount ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return svcPrflCount;
}

unsigned int
EntityManager::getCounter ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return counter;
}

void
EntityManager::addSvcDataRecord (const Util::DataRecord& svcDataRecord)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  svcRecordQueue.push (svcDataRecord);
}

bool
EntityManager::getSvcDataRecord (Util::DataRecord *svcDataRecord)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = svcRecordQueue.size () > 0;
  if (flag)
    {
      *svcDataRecord = svcRecordQueue.front ();
      svcRecordQueue.pop ();
      ++counter;

      if (counter == svcPrflCount)
        {
          counter = 0;
          svcPrflCount = 0;
        }
    }

  return flag;
}

void
EntityManager::addIncomingMsg (const std::string& msg, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      incomingClientMsgQueue.push (msg);
      break;
    }
  case ENVIR_MSG :
    {
      incomingEnvironmentMsgQueue.push (msg);
      break;
    }
  case ENTITY_MSG :
    {
      incomingEntityMsgQueue.push (msg);
      break;
    }
  }
}

bool
EntityManager::getIncomingMsg (std::string *msg, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = incomingClientMsgQueue.size () > 0;
      if (flag)
        {
          (*msg) = incomingClientMsgQueue.front ();
          incomingClientMsgQueue.pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      flag = incomingEnvironmentMsgQueue.size () > 0;
      if (flag)
        {
          (*msg) = incomingEnvironmentMsgQueue.front ();
          incomingEnvironmentMsgQueue.pop ();
        }
      break;
    }
  case ENTITY_MSG :
    {
      flag = incomingEntityMsgQueue.size () > 0;
      if (flag)
        {
          (*msg) = incomingEntityMsgQueue.front ();
          incomingEntityMsgQueue.pop ();
        }
      break;
    }
  }

  return flag;
}

void
EntityManager::addIncomingDataSeq (const std::vector <std::string>& dataSeq, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      incomingClientDataSeqQueue.push (dataSeq);
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      /// TODO
      break;
    }
  }
}

bool
EntityManager::getIncomingDataSeq (std::vector <std::string> *dataSeq, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = incomingClientDataSeqQueue.size () > 0;
      if (flag)
        {
          (*dataSeq) = incomingClientDataSeqQueue.front ();
          incomingClientDataSeqQueue.pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      break;
    }
  }

  return flag;
}

void
EntityManager::addIncomingDataRec (const Util::DataRecord& dataRec, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      incomingClientDataRecQueue.push (dataRec);
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      /// TODO
      break;
    }
  }
}

bool
EntityManager::getIncomingDataRec (Util::DataRecord *dataRec, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = incomingClientDataRecQueue.size () > 0;
      if (flag)
        {
           (*dataRec) = incomingClientDataRecQueue.front ();
          incomingClientDataRecQueue.pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      /// TODO
      break;
    }
  }

  return flag;
}

void
EntityManager::addOutgoingMsg (const std::string& msg, const std::string& msgType,
    unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      outgoingClientMsgQueue.push (msg);
      outgoingClientMsg_typeQueue.push (msgType);
      break;
    }
  case ENVIR_MSG :
    {
      outgoingEnvironmentMsgQueue.push (msg);
      outgoingEnvironmentMsg_typeQueue.push (msgType);
      break;
    }
  }
}

bool
EntityManager::getOutgoingMsg (std::string *msg, std::string *msgType, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = outgoingClientMsgQueue.size () > 0;
      if (flag)
        {
          (*msg) = outgoingClientMsgQueue.front ();
          outgoingClientMsgQueue.pop ();
          (*msgType) = outgoingClientMsg_typeQueue.front ();
          outgoingClientMsg_typeQueue. pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      flag = outgoingEnvironmentMsgQueue.size () > 0;
      if (flag)
        {
          (*msg) = outgoingEnvironmentMsgQueue.front ();
          outgoingEnvironmentMsgQueue.pop ();
          (*msgType) = outgoingEnvironmentMsg_typeQueue.front ();
          outgoingEnvironmentMsg_typeQueue.pop ();
        }
      break;
    }
  }

  return flag;
}

void
EntityManager::addOutgoingDataSeq (const std::vector <std::string>& dataSeq,
    const std::string& msgType, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      outgoingClientDataSeqQueue.push (dataSeq);
      outgoingClientDataSeq_typeQueue.push (msgType);
      break;
    }
  case ENVIR_MSG :
    {
      outgoingEnvironmentDataSeqQueue.push (dataSeq);
      outgoingEnvironmentDataSeq_typeQueue.push (msgType);
      break;
    }
/*
  case ENTITY_MSG :
    {
      break;
    }
*/
  }
}

bool
EntityManager::getOutgoingDataSeq (std::vector <std::string> *dataSeq,
    std::string *msgType, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = outgoingClientDataSeqQueue.size () > 0;
      if (flag)
        {
          (*dataSeq) = outgoingClientDataSeqQueue.front ();
          outgoingClientDataSeqQueue.pop ();
          (*msgType) = outgoingClientDataSeq_typeQueue.front ();
          outgoingClientDataSeq_typeQueue.pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      flag = outgoingEnvironmentDataSeqQueue.size ();
      if (flag)
        {
          (*dataSeq) = outgoingEnvironmentDataSeqQueue.front ();
          outgoingEnvironmentDataSeqQueue.pop ();
          (*msgType) = outgoingEnvironmentDataSeq_typeQueue.front ();
          outgoingEnvironmentDataSeq_typeQueue.pop ();
        }
      break;
    }
  }

  return flag;
}

void
EntityManager::addOutgoingDataRec (const Util::DataRecord& dataRec, const std::string& msgType,
    unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      outgoingClientDataRecQueue.push (dataRec);
      outgoingClientDataRec_typeQueue.push (msgType);
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  }
}

bool
EntityManager::getOutgoingDataRec (Util::DataRecord *dataRec, std::string *msgType,
    unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = outgoingClientDataRecQueue.size () > 0;
      if (flag)
        {
          (*dataRec) = outgoingClientDataRecQueue.front ();
          outgoingClientDataRecQueue.pop ();
          (*msgType) = outgoingClientDataRec_typeQueue.front ();
          outgoingClientDataRec_typeQueue.pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  }

  return flag;
}

void
EntityManager::addOutgoingEntityMsg (const std::string& msg, const std::string& msgType,
    const std::string& destId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  outgoingEntityMsgQueue.push (msg);
  outgoingEntityMsg_typeQueue.push (msgType);
  outgoingEntityMsg_destQueue.push (destId);
}

bool
EntityManager::getOutgoingEntityMsg (std::string *msg, std::string *msgType, std::string *destId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;
  flag = outgoingEntityMsgQueue.size () > 0;

  if (flag)
    {
      (*msg) = outgoingEntityMsgQueue.front ();
      outgoingEntityMsgQueue.pop ();
      (*msgType) = outgoingEntityMsg_typeQueue.front ();
      outgoingEntityMsg_typeQueue.pop ();
      (*destId) = outgoingEntityMsg_destQueue.front ();
      outgoingEntityMsg_destQueue.pop ();
    }

  return flag;
}

void
EntityManager::addOutgoingEntityDataSeq (const std::vector <std::string>& dataSeq,
    const std::string& msgType, const std::string& destId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  outgoingEntityDataSeqQueue.push (dataSeq);
  outgoingEntityDataSeq_typeQueue.push (msgType);
  outgoingEntityDataSeq_destQueue.push (destId);
}

bool
EntityManager::getOutgoingEntityDataSeq (std::vector <std::string> *dataSeq, std::string *msgType,
    std::string *destId)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = outgoingEntityDataSeqQueue.size () > 0;
  if (flag)
    {
      (*dataSeq) = outgoingEntityDataSeqQueue.front ();
      outgoingEntityDataSeqQueue.pop ();
      (*msgType) = outgoingEntityDataSeq_typeQueue.front ();
      outgoingEntityDataSeq_typeQueue.pop ();
      (*destId) = outgoingEntityDataSeq_destQueue.front ();
      outgoingEntityDataSeq_destQueue.pop ();
    }

  return flag;
}

void
EntityManager::addNewTask (std::vector <std::string>& taskSeq, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      pendingClientTaskQueue.push (taskSeq);
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      pendingEntityTaskQueue.push (taskSeq);
      break;
    }
  }
}

bool
EntityManager::getPendingTask (std::vector <std::string> *taskSeq, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = pendingClientTaskQueue.size () > 0;
      if (flag)
        {
          (*taskSeq) = pendingClientTaskQueue.front ();
          pendingClientTaskQueue.pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      flag = pendingEntityTaskQueue.size () > 0;
      if (flag)
        {
          (*taskSeq) = pendingEntityTaskQueue.front ();
          pendingEntityTaskQueue.pop ();
        }
      break;
    }
  }

  return flag;
}

void
EntityManager::addNewCollaborativeTask (Util::DataRecord& dataRec, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  switch (type)
  {
  case CLIENT_MSG :
    {
      pendingClientCollaborativeTaskQueue.push (dataRec);
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      /// TODO
      break;
    }
  }
}

bool
EntityManager::getPendingCollaborativeTask (Util::DataRecord *dataRec, unsigned int type)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  bool flag;

  flag = false;
  switch (type)
  {
  case CLIENT_MSG :
    {
      flag = pendingClientCollaborativeTaskQueue.size () > 0;
      if (flag)
        {
          (*dataRec) = pendingClientCollaborativeTaskQueue.front ();
          pendingClientCollaborativeTaskQueue.pop ();
        }
      break;
    }
  case ENVIR_MSG :
    {
      /// TODO
      break;
    }
  case ENTITY_MSG :
    {
      /// TODO
      break;
    }
  }

  return flag;
}

bool
EntityManager::sendEntitySyncMsg (const std::string& prefix, const std::string& body,
    std::string *reply)
{
  std::string msg;
  bool flag;

  msg.append (prefix);
  msg.append (DELIM);
  msg.append (body);
  flag = backendInterfacer.sendSyncMsg (msg, reply);
  if (!flag)
    {
      if (logFlag)
        {
          std::ostringstream oss;

          oss << "EntityManager: " << "failed to send message to entity. Connection lost.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }
    }

  return flag;
}

bool
EntityManager::sendEntitySyncMsg (const std::string& prefix, const std::string& body, char *reply,
      unsigned int size)
{
  std::string msg;
  bool flag;
  std::ostringstream oss;

  msg.append (prefix);
  msg.append (DELIM);
  msg.append (body);
  flag = backendInterfacer.sendSyncMsg (msg, reply, size);
  if (!flag)
    {
      if (logFlag)
        {
          oss << "EntityManager: " << "failed to send message to entity. Connection lost.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }
    }

  return flag;
}

void
EntityManager::setErrState (const bool& flag)
{
  IceUtil::Mutex::Lock sync (dataMutex);

  errState = flag;
}

bool
EntityManager::getErrState ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  return errState;
}

void
EntityManager::clearPendingClientTasks ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  while (!pendingClientTaskQueue.empty ())
      pendingClientTaskQueue.pop ();
}

void
EntityManager::clearPendingClientCollaborativeTasks ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  while (!pendingClientCollaborativeTaskQueue.empty ())
      pendingClientCollaborativeTaskQueue.pop ();
}

void
EntityManager::clearPendingEntityTasks ()
{
  IceUtil::Mutex::Lock sync (dataMutex);

  while (!pendingEntityTaskQueue.empty ())
      pendingEntityTaskQueue.pop ();
}

int
EntityManager::dispatchToEntity (std::string& data, std::string *reply, bool *taskFlag)
{
  DataFormatConverter ft;
  char dataBuff [BUFFSIZE];
  char replyBuff [BUFFSIZE];
  std::string type;
  std::string prefix;
  std::string tmp;
  Data::Param param;
  Data::Resource resource;
  Data::Task task;

  ft.getPrefix (&data, &type);
  (*reply) = "";
  /// If it is a Param
  if (type == PARAM)
    {
      std::cout << "Dispatchiiiiiiiiiiiiing a PARAM" << std::endl;
      /// Re-format to a char array (For socket compatibility)
      if (!ft.formatParam (&param, &data, false)
          || !ft.formatParam (&param, dataBuff, BUFFSIZE, true))
        {
          (*reply) = BAD_PARAM;
          return FALSE;
        }
      else
        {
          /// If unable to update PARAM
          if (serviceProfiler.updateCtrlParam (&param) != OK)
            {
              stateLog ("REG", "Error", "Failed to parse update parameter.");
              ft.formatParam (&param, reply, true, DENY_PARAM);
              return FALSE;
            }
          /// Signal the entity to prepare for data receive and then send data itself
          if (!sendEntitySyncMsg (DATA, type, &tmp)
              || !backendInterfacer.sendSyncMsg (dataBuff, BUFFSIZE, replyBuff, BUFFSIZE))
            {
              return ERR;
            }
          /// Re-formatting back to send reply to client
          ft.getPrefix (replyBuff, BUFFSIZE, &prefix);
          ft.formatParam (&param, replyBuff, BUFFSIZE, false);
          /// Attach the new prefix to the reply PARAM
          ft.formatParam (&param, reply, true, prefix);
          /// If PARAM update was denied or service profiler failed to update it
          if (prefix == DENY_PARAM)
              return FALSE;
        }
    }
  /// If it is a Resource
  else if (type == RESOURCE)
    {
      /// Re-format to a char array (For socket compatibility)
      if (!ft.formatResource (&resource, &data, false)
          || !ft.formatResource (&resource, dataBuff, BUFFSIZE, true))
        {
          (*reply) = BAD_RES;
          return FALSE;
        }
      else
        {
          /// If unable to update RESOURCE
          if (serviceProfiler.updateResource (&resource) != OK)
            {
              ft.formatResource (&resource, reply, true, DENY_RES);
              return FALSE;
            }
          /// Signal the entity to prepare for data receive and then send data itself
          if (!sendEntitySyncMsg (DATA, type, &tmp)
              || !backendInterfacer.sendSyncMsg (dataBuff, BUFFSIZE, replyBuff, BUFFSIZE))
            {
              return ERR;
            }
          /// Re-formatting back to send reply to client
          ft.getPrefix (replyBuff, BUFFSIZE, &prefix);
          ft.formatResource (&resource, replyBuff, BUFFSIZE, false);
          /// Attach the new prefix to the reply RESOURCE
          ft.formatResource (&resource, reply, true, prefix);
          /// If RESOURCE update was denied or service profiler failed to update it
          if (prefix == DENY_RES)
              return FALSE;
        }
    }
  else if (type == TSK)
    {
      std::cout << "Dispatchiiiiiiiiiiiiing a TASK" << std::endl;
      /// Re-format to a char array (For socket compatibility)
      if (!ft.formatTask (&task, &data, false)
          || !ft.formatTask (&task, dataBuff, BUFFSIZE, true))
        {
          (*reply) = BAD_TSK;
          return FALSE;
        }
      else
        {
          /// If all dependencies of the TASK are not satisfied
          if (serviceProfiler.checkAllSet (task.serviceid ()) != OK)
            {
              std::cout << "inCOMMMMMMMMMMMMMPLETETE a TASK" << std::endl;

              ft.formatTask (&task, reply, true, DENY_TSK);
              return FALSE;
            }
          /// Reseting service dependency
          serviceProfiler.resetServiceDependency (task.serviceid ());
          /// Signal the entity to prepare for data receive and then send data itself
          if (!sendEntitySyncMsg (DATA, type, &tmp)
              || !backendInterfacer.sendSyncMsg (dataBuff, BUFFSIZE, replyBuff, BUFFSIZE))
            {
              return ERR;
            }
          /// Re-formatting back to send reply to client
          ft.getPrefix (replyBuff, BUFFSIZE, &prefix);
          ft.formatTask (&task, replyBuff, BUFFSIZE, false);
          ft.formatTask (&task, reply, true, prefix);
          /// If TASK update was denied or service was not ready
          if (prefix == DENY_TSK)
            {
              return FALSE;
            }
          /// Signal the acceptance to carry out the task
          else
            {
              (*taskFlag) = true;
            }
        }
    }
  /// Unknown data type
  else
    {
      (*reply) = UNKNOWN_DATA;
      return FALSE;
    }

  return OK;
}

void
EntityManager::releaseReset ()
{
  clearPendingClientTasks ();
  clearPendingClientCollaborativeTasks ();
  clearPendingEntityTasks ();
  clearAllData ();
  setCheckedOut (false);
  setReleased (false);
  toggleCollaborationMode (false);
  toggleSlaveMode (false);
}

void
EntityManager::resetAll ()
{
  releaseReset ();
  setConnectSignal (false);
  setEntityOnline (false);
  setRequested (false);
  toggleStop (false);
  ///FIXME Temporary fix
  setInitiator (false);
  /// Closing connection with entity
  closeEntityConn ();
  /// Clearing basic and service profiles
  serviceProfiler.clear ();
  basicPrfl.Clear ();
}

void
EntityManager::clearAllData ()
{
  IceUtil::Mutex::Lock sync (dataMutex);
  /// Clearing message and data queues
  while (!svcRecordQueue.empty ())
      svcRecordQueue.pop ();

  while (!incomingClientMsgQueue.empty ())
      incomingClientMsgQueue.pop ();

  while (!incomingClientDataSeqQueue.empty ())
      incomingClientDataSeqQueue.pop ();

  while (!incomingClientDataRecQueue.empty ())
      incomingClientDataRecQueue.pop ();

  while (!incomingEnvironmentMsgQueue.empty ())
      incomingEnvironmentMsgQueue.pop ();

  while (!incomingEntityMsgQueue.empty ())
      incomingEntityMsgQueue.pop ();

  while (!outgoingClientMsgQueue.empty ())
    {
      outgoingClientMsgQueue.pop ();
      outgoingClientMsg_typeQueue.pop ();
    }

  while (!outgoingClientDataSeqQueue.empty ())
    {
      outgoingClientDataSeqQueue.pop ();
      outgoingClientDataSeq_typeQueue.pop ();
    }

  while (!outgoingClientDataRecQueue.empty ())
    {
      outgoingClientDataRecQueue.pop ();
      outgoingClientDataRec_typeQueue.pop ();
    }

  while (!outgoingEnvironmentMsgQueue.empty ())
    {
      outgoingEnvironmentMsgQueue.pop ();
      outgoingEnvironmentMsg_typeQueue.pop ();
    }

  while (!outgoingEnvironmentDataSeqQueue.empty ())
    {
      outgoingEnvironmentDataSeqQueue.pop ();
      outgoingEnvironmentDataSeq_typeQueue. pop ();
    }

  while (!outgoingEntityMsgQueue.empty ())
    {
      outgoingEntityMsgQueue.pop ();
      outgoingEntityMsg_typeQueue.pop ();
      outgoingEntityMsg_destQueue.pop ();
    }

  while (!outgoingEntityDataSeqQueue.empty ())
    {
      outgoingEntityMsgQueue.pop ();
      outgoingEntityDataSeq_typeQueue.pop ();
      outgoingEntityDataSeq_destQueue.pop ();
    }
}

void
EntityManager::closeEntityConn ()
{
  backendInterfacer.closeConn ();
}

/*! \brief This function is responsible for final clean-up and shutdown.
 */
void
EntityManager::close ()
{
  backendInterfacer.closeConn ();
}

void
EntityManager::stateLog (const std::string& state, const std::string& type, const std::string& msg)
{
  std::ostringstream oss;

  if (logFlag)
    {
      oss << "EntityManager: " << state << ": " << msg;
      iceLogger_Ptr->trace (type, oss.str ());
    }
}
