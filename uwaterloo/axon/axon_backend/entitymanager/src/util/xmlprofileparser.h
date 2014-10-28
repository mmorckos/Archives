/*
 * xmlprofileparser.h
 *
 *  Created on: 2012-04-03
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef XMLPROFILEPARSER_H_
#define XMLPROFILEPARSER_H_

#include "parser.h"
#include "./third_party/pugixml.hpp"
#include "../protobufdata/entityprofile.pb.h"

class XmlProfileParser : public Parser
{

public:
  XmlProfileParser ();
  virtual ~XmlProfileParser ();

  bool parse (Data::BasicProfile *basicProfile);

protected:
  pugi::xml_document inFile;
};

#endif /* XMLPROFILEPARSER_H_ */
