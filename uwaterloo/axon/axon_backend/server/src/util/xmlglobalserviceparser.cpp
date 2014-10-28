/*
 * globalservicexmlparser.cpp
 *
 *  Created on: 2012-04-11
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

//#include <map>

#include "xmlglobalserviceparser.h"
#include "dataformatconverter.h"

XmlGlobalServiceParser::XmlGlobalServiceParser () :
  globalServicesData (0)
{}

XmlGlobalServiceParser::~XmlGlobalServiceParser ()
{}

void
XmlGlobalServiceParser::init (std::vector <std::string> *l)
{
  globalServicesData = l;
}

bool
XmlGlobalServiceParser::parse ()
{
  if (!inFile.load_file (fileName.c_str ()))
    {
      internalStatus = "Unable to parse XML file.";
      return false;
    }

  globalServices = inFile.child ("GlobalProfile");
  globalServiceCount = globalServices.attribute ("count").as_int ();

  if (globalServiceCount == 0)
    {
      internalStatus = "No GlobalService exists or GlobalService <count> attribute was not set.";
      return false;
    }

  return parseGlobalServices ();
}

bool
XmlGlobalServiceParser::parseGlobalServices ()
{
  pugi::xml_node service;
  DataFormatConverter ft;
  unsigned int serviceCount;
  std::string value;

  serviceCount = 0;

  for (pugi::xml_node globalServiceXmlNode = globalServices.first_child ();
      globalServiceXmlNode; globalServiceXmlNode = globalServiceXmlNode.next_sibling (),
      ++serviceCount)
    {
      Data::GlobalService globalServiceData;

      value = "";
      value = globalServiceXmlNode.attribute ("name").value ();
      if (value.size () == 0)
        {
          internalStatus = "Missing <name> attribute in GlobalService.";
          return false;
        }
      else
        {
          globalServiceData.set_name (value);
        }

      for (pugi::xml_node entityRecordXmlNode = globalServiceXmlNode.first_child ();
          entityRecordXmlNode; entityRecordXmlNode = entityRecordXmlNode.next_sibling ())
        {
          Data::GlobalServiceTuple *globalServiceTupleData;

          globalServiceTupleData = globalServiceData.add_tuple ();

          value = "";
          value = entityRecordXmlNode.attribute ("category").value ();
          if (value.size () == 0)
            {
              internalStatus = "Missing <category> attribute in Entity record.";
              return false;
            }
          else
            {
              if (value == "ROBOT")
                {
                  globalServiceTupleData->set_category (Data::GlobalServiceTuple_Category_ROBOT);
                }
              else if (value == "MACHINE")
                {
                  globalServiceTupleData->set_category (Data::GlobalServiceTuple_Category_MACHINE);
                }
              else
                {
                  internalStatus = "Unknown <category> in Entity.";
                  return false;
                }
            }

          value = "";
          value = entityRecordXmlNode.attribute ("type").value ();
          if (value.size () == 0)
            {
              internalStatus = "Missing <type> type in Entity.";
              return false;
            }
          else
            {
              if (value == "MOBILE")
                {
                  globalServiceTupleData->set_type (Data::GlobalServiceTuple_Type_MOBILE);
                }
              else if (value == "STATIONARY")
                {
                  globalServiceTupleData->set_type (Data::GlobalServiceTuple_Type_STATIONARY);
                }
              else
                {
                  internalStatus = "Unknown <type> in Entity.";
                  return false;
                }
            }

          value = "";
          value = entityRecordXmlNode.attribute ("id").value ();
          if (value.size () == 0)
            {
              internalStatus = "Missing <id> type in Entity.";
              return false;
            }
          else
            {
              globalServiceTupleData->set_id (value);
            }

          value = "";
          value = entityRecordXmlNode.attribute ("service_global_id").value ();
          if (value.size () == 0)
            {
              internalStatus = "Missing <service_global_id> type in Entity.";
              return false;
            }
          else
            {
              globalServiceTupleData->set_serviceglobalid (value);
            }

          value = "";
          value = entityRecordXmlNode.attribute ("preced").value ();
          if (value.size () == 0)
            {
              internalStatus = "Missing <preced> type in Entity.";
              return false;
            }
          else
            {
                globalServiceTupleData->set_preced (
                      entityRecordXmlNode.attribute ("preced").as_int ());
            }

          for (pugi::xml_node keyValuePairXmlNode = entityRecordXmlNode.first_child ();
              keyValuePairXmlNode; keyValuePairXmlNode = keyValuePairXmlNode.next_sibling ())
            {
              value = "";
              value = keyValuePairXmlNode.attribute ("param_name").value ();
              if (value.size () == 0)
                {
                  internalStatus = "Missing <param_name> attribute in key-value of an Entity.";
                  return false;
                }
              else
                {
                  globalServiceTupleData->add_paramname (value);
                }

              value = "";
              value = keyValuePairXmlNode.attribute ("param_value").value ();
              if (value.size () == 0)
                {
                  internalStatus = "Missing <param_name> attribute in key-value of an Entity.";
                  return false;
                }
              else
                {
                  globalServiceTupleData->add_paramvalue (value);
                }
            }
        }

      if (!ft.formatGlobalService (&globalServiceData, &value, true))
        {
          internalStatus = ft.getInternalStatus ();
          return false;
        }
      else
        {
          globalServicesData->push_back (value);
        }
    }

  if (serviceCount != globalServiceCount)
    {
      internalStatus = "GlobalService record count mismatch.";
      return false;
    }

  return true;
}
