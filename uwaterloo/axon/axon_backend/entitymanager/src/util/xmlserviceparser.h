/*
 * serviceparser.h
 *
 *  Created on: 2011-11-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef XMLSERVICEPARSER_H_
#define XMLSERVICEPARSER_H_

#include <vector>
#include "parser.h"
#include "./third_party/pugixml.hpp"
#include "../protobufdata/ctrlparam.pb.h"
#include "../protobufdata/resource.pb.h"
#include "../protobufdata/service.pb.h"

class XmlServiceParser : public Parser
{

public:
  XmlServiceParser ();

  void init (std::vector <Data::Service> *l0, std::vector <Data::CtrlParam> *l1,
      std::vector <Data::Resource> *l2);
  bool parse ();

protected:
  pugi::xml_document inFile;
  pugi::xml_node serviceProfile;
  unsigned int serviceCount;
  std::vector <Data::Service> *serviceList;     /*!< Service list that will store services data. */
  std::vector <Data::CtrlParam> *ctrlParamList; /*!< CtrlParam list that will store ctrlParams data. */
  std::vector <Data::Resource> *resourceList;   /*!< Resource list that will store resources data. */
  unsigned int expectedCtrlParamCount;  /*!< Expected number of parameter for a service. */
  unsigned int expectedResourceCount;   /*!< Expected number of parameter for a service. */

  bool parseServices ();
  bool parseCtrlParams (pugi::xml_node parameters, unsigned int serviceId,
      const std::string& globalServiceId, unsigned int *ctrlParamId, unsigned int expected);
  bool parseResources (pugi::xml_node resources, unsigned int serviceId,
      const std::string& globalServiceId, unsigned int expected);
};

#endif /* SERVICEPARSER_H_ */
