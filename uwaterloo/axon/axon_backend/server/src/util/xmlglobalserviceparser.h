/*
 * globalservicexmlparser.h
 *
 *  Created on: 2012-04-11
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef XMLGLOBALSERVICEPARSER_H_
#define XMLGLOBALSERVICEPARSER_H_

#include <vector>

#include "parser.h"
#include "./third_party/pugixml.hpp"

class XmlGlobalServiceParser : public Parser
{
public:
  XmlGlobalServiceParser ();
  virtual ~XmlGlobalServiceParser ();

  void init (std::vector <std::string> *l);
  bool parse ();

protected:
  pugi::xml_document inFile;
  pugi::xml_node globalServices;
  unsigned int globalServiceCount;
  std::vector <std::string> *globalServicesData;
  enum Category {ROBOT, MACHINE};
  enum Type {MOBILE, STATIONARY};

  bool parseGlobalServices ();
};

#endif /* XMLGLOBALSERVICEPARSER_H_ */
