/*
 * serviceparser.h
 *
 *  Created on: 2011-11-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 *
 *  DESFUNCT
 */

#ifndef SERVICEPARSER_H_
#define SERVICEPARSER_H_

#include <vector>

#include "parser.h"
#if defined (__linux__)
#include "../protobufdata/ctrlparam.pb.h"
#include "../protobufdata/resource.pb.h"
#include "../protobufdata/service.pb.h"
#elif defined (_WIN32)
#include "..\\structinfo\\service.pb.h"
#include "..\\structinfo\\ctrlparam.pb.h"
#include "..\\structinfo\\param.pb.h"
#include "..\\structinfo\\resource.pb.h"
#endif

const unsigned int MIN_SVC_TOKEN_COUNT = 2;
const unsigned int MIN_CTRLPARAM_TOKEN_COUNT = 3;
const unsigned int MIN_RESOURCE_TOKEN_COUNT = 2;
const unsigned int MINSVC_COUNT = 1;

class ServiceParser : public Parser
{

public:
  ServiceParser ();

  void init (std::vector <Data::Service> *l0, std::vector <Data::CtrlParam> *l1,
      std::vector <Data::Resource> *l2);
  bool parse ();

protected:
  std::vector <Data::Service> *serviceList;     /*!< Service list that will store services data. */
  std::vector <Data::CtrlParam> *ctrlParamList; /*!< CtrlParam list that will store ctrlParams data. */
//  std::vector <Data::Param> *paramList;         /*!< Param list that will store params data. */
  std::vector <Data::Resource> *resourceList;   /*!< Resource list that will store resources data. */
  std::string token;  /*!< Stores most recent parsed token. */
  std::string value;  /*!< Stores most recent parsed value. */
  unsigned int count;          /*!< Stores the number of data items parsed so far. */
  unsigned int expectedCtrlParamCount;  /*!< Expected number of parameter for a service. */
  unsigned int expectedResourceCount;   /*!< Expected number of parameter for a service. */
  unsigned int ctrlParamCount; /*!< Stores the number of CtrlParams of a service scanned so far. */
  unsigned int resourceCount;  /*!< Stores the number of Resources of a service scanned so far. */
  unsigned int serviceId;      /*!< Current Service ID. */
  unsigned int ctrlParamId;    /*!< Current CtrlParam ID. */
//  unsigned int paramId;        /*!< Current Param ID. */
  unsigned int resourceId;     /*!< Current Resource ID. */

  bool parse (unsigned int key);
  bool packService (Data::Service *service);
  bool packCtrlParam (Data::CtrlParam *ctrlParam);
  bool packResource (Data::Resource *resource);
  bool parseResource ();
  bool eofCheck ();

  void setUnknownErrMsg (const std::string& type, const std::string& token)
  {
    std::ostringstream oss;

    oss << "Unknown " << type << " token: " << token;
    internalStatus = oss.str ();
  }
};

#endif /* SERVICEPARSER_H_ */
