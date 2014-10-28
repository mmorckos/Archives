/*
 * entitytopics.ice
 *
 * ICE IDL definition for inter-server communications.
 *
 * Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef ENTITYTOPICS_ICE_
#define ENTITYTOPICS_ICE_

#include <commutil.ice>

module EntityComm
{
  interface EntityToFrontEndComm
  {
    void entityBasicPrflUpdate (string entityId, string entityType, string msg);
    void entitySvcRecord (string entityId, string entityType, int recordId, int count, \
      CommUtil::DataRecord svcRecord);
    void entityMsg (string entityId, string entityType, string msgType, string msg);
    void entityMsgDataSeq (string clientId, string entityType, string msgType, \
      CommUtil::DataSeq dataSeq);
    void entityMsgDataRec (string clientId, string entityType, string msgType, \
      CommUtil::DataRecord dataRec);
    void entityUpdate (string entityId, string entityType, string msgType, string msg);
  };
};

#endif // ENTITYTOPICS_ICE_
