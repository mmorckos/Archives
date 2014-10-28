/*
 * entitytofrontendcomm_i.h
 *
 *  Created on: 2012-01-10
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef ENTITYTOFRONTENDCOMM_I_H_
#define ENTITYTOFRONTENDCOMM_I_H_

#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include "clientmonitor.h"
#include "entitytopics.h"

class EntityToFrontEndComm_I : virtual public EntityComm::EntityToFrontEndComm
{

public:
  EntityToFrontEndComm_I (const ClientMonitorPtr& cm);

  virtual void entityBasicPrflUpdate (const std::string& entityId, const std::string& entityType,
      const std::string& msg, const Ice::Current& curr);
  virtual void entitySvcRecord (const std::string& entityId, const std::string& entityType,
      int recordId, int count, const CommUtil::DataRecord& svcRecord, const Ice::Current& curr);
  virtual void entityMsg (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const std::string& msg, const Ice::Current& curr);
  virtual void entityMsgDataSeq (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const CommUtil::DataSeq& dataSeq, const Ice::Current& curr);
  virtual void entityMsgDataRec (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const CommUtil::DataRecord& dataRec, const Ice::Current& curr);
  virtual void entityUpdate (const std::string& entityId, const std::string& entityType,
      const std::string& msgType, const std::string& msg, const Ice::Current& curr);

protected:
  const ClientMonitorPtr clientMonitor_Ptr;
};
typedef IceUtil::Handle <EntityToFrontEndComm_I> EntityToFrontEndComm_IPtr;

#endif /* ENTITYTOFRONTENDCOMM_I_H_ */
