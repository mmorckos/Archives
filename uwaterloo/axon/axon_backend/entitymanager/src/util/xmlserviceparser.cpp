/*
 * serviceparser.cpp
 *
 *  Created on: 2011-11-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "xmlserviceparser.h"
#include "../common/global.h"
#include "../common/status.h"

XmlServiceParser::XmlServiceParser ():
  expectedCtrlParamCount (0),
  expectedResourceCount (0)
{}

void
XmlServiceParser::init (std::vector <Data::Service> *l0, std::vector <Data::CtrlParam> *l1,
    std::vector <Data::Resource> *l2)
{
  serviceList = l0;
  ctrlParamList = l1;
  resourceList = l2;
}

/*! \brief Parses the given serviceList file and extracts all serviceXmlNode information.
 *
 * \return true if file has a valid format. false otherwise.
 */
bool
XmlServiceParser::parse ()
{
  if (!inFile.load_file (fileName.c_str ()))
    {
      internalStatus = "Unable to parse XML file.";
      return false;
    }

  serviceProfile = inFile.child ("ServiceProfile");
  serviceCount = serviceProfile.attribute ("count").as_int ();

  if (serviceCount == 0)
    {
      internalStatus = "No Service exists or Service <count> attribute was not set.";
      return false;
    }

  return parseServices ();
}

bool
XmlServiceParser::parseServices ()
{
  pugi::xml_node parameters;
  pugi::xml_node resources;
  unsigned int serviceId;
  unsigned int ctrlParamId;
  unsigned int expectedCtrlParamCount;  /*!< Expected number of parameter for a service. */
  unsigned int expectedResourceCount;   /*!< Expected number of parameter for a service. */
  std::string value;

  serviceId = 0;
  ctrlParamId = 0;

  for (pugi::xml_node serviceXmlNode = serviceProfile.first_child ();
      serviceXmlNode; serviceXmlNode = serviceXmlNode.next_sibling (), ++serviceId)
    {
      Data::Service serviceData;

      serviceData.set_id (serviceId);

      value = serviceXmlNode.attribute ("name").value ();
      if (value.size () == 0)
        {
          internalStatus = "Missing <name> attribute in Service.";
          return false;
        }
      else
        {
          serviceData.set_name (value);
        }

      value = "";
      value = serviceXmlNode.attribute ("type").value ();
      if (value.size () == 0)
        {
          internalStatus = "Missing <type> attribute in Service.";
          return false;
        }
      else
        {
          if (value == "SMPL")
            {
              serviceData.set_type (Data::Service_Type_SMPL);
            }
          else if (value == "CPLX")
            {
              serviceData.set_type (Data::Service_Type_CPLX);
            }
          else if (value == "ITRAV")
            {
              serviceData.set_type (Data::Service_Type_ITRAV);
            }
          else
            {
              internalStatus = "Unknown <type> value in Service.";
              return false;
            }
        }

      value = "";
      value = serviceXmlNode.attribute ("active").value ();
      if (value.size () > 0)
          serviceData.set_isactive (serviceXmlNode.attribute ("active").as_bool ());

      value = "";
      value = serviceXmlNode.attribute ("descr").value ();
      serviceData.set_descr (value);

      value = "";
      value = serviceXmlNode.attribute ("notes").value ();
      serviceData.set_notes (value);

      value = "";
      value = serviceXmlNode.attribute ("global_id").value ();
      serviceData.set_globalid (value);

      value = "";
      value = serviceXmlNode.attribute ("blocking").value ();
      if (value.size () > 0)
          serviceData.set_isblocking (serviceXmlNode.attribute ("blocking").as_bool ());

      value = "";
      value = serviceXmlNode.attribute ("pausable").value ();
      if (value.size () > 0)
          serviceData.set_ispausable (serviceXmlNode.attribute ("pausable").as_bool ());

      value = "";
      value = serviceXmlNode.attribute ("service_list").value ();
      if (value.size () > 0)
        {
          char str [BUFFSIZE];
          const char *tmp =  value.c_str ();
          strncpy (str, tmp, BUFFSIZE);
          char *pch = strtok (str, " ");

          while (pch != NULL)
            {
              serviceData.add_servicelist (atoi (pch));
              pch = strtok (NULL, " ");
            }
        }

      expectedCtrlParamCount = serviceXmlNode.attribute ("parameter_count").as_uint ();
      if (expectedCtrlParamCount > 0)
        {
          parameters = serviceXmlNode.child ("Parameters");
          if (!parseCtrlParams (parameters, serviceId, serviceData.globalid (), &ctrlParamId,
              expectedCtrlParamCount))
            {
              return false;
            }
        }

      serviceList->push_back (serviceData);
    }

  /// If the expected parameter count is not the same as parsed from service
  if (serviceId != serviceCount)
    {
      internalStatus = "Mismatch in Service count.";
      return false;
    }

  return true;
}

bool
XmlServiceParser::parseCtrlParams (pugi::xml_node parameters, unsigned int serviceId,
    const std::string& globalServiceId, unsigned int *ctrlParamId, unsigned int expected)
{
  pugi::xml_node keyValuePairs;
  unsigned int paramCount;
  unsigned int tmpCount;
  std::string value;

  paramCount = 0;
  tmpCount = 0;

  for (pugi::xml_node paramXmlNode = parameters.first_child (); paramXmlNode;
      paramXmlNode = paramXmlNode.next_sibling ())
    {
      Data::CtrlParam ctrlParamData;

      ctrlParamData.set_id (*(ctrlParamId));
      ctrlParamData.set_serviceid (serviceId);
      ctrlParamData.set_globalsvcid (globalServiceId);

      value = paramXmlNode.attribute ("name").value ();
      if (value.size () == 0)
        {
          internalStatus = "Missing <name> attribute in CtrlParam.";
          return false;
        }
      else
        {
          ctrlParamData.set_name (value);
        }

      value = "";
      value = paramXmlNode.attribute ("type").value ();
      if (value.size () == 0)
        {
          internalStatus = "Missing <type> attribute in CtrlParam.";
          return false;
        }
      else
        {
          if (value == "INPUT")
            {
              ctrlParamData.set_type (Data::CtrlParam_Type_INPUT);
            }
          else if (value == "OUTPUT")
            {
              ctrlParamData.set_type (Data::CtrlParam_Type_INPUT);
            }
          else if (value == "CONFIG")
            {
              ctrlParamData.set_type (Data::CtrlParam_Type_CONFIG);
            }
          else if (value == "FDBK")
            {
              ctrlParamData.set_type (Data::CtrlParam_Type_FDBK);
            }
          else
            {
              internalStatus = "Unknown <type> value in CtrlParam.";
              return false;
            }
        }

      value = "";
      value = paramXmlNode.attribute ("data_type").value ();
      if (value.size () == 0)
        {
          internalStatus = "Missing <data_type> attribute in CtrlParam.";
          return false;
        }
      else
        {
          if (value == "INT")
            {
              ctrlParamData.set_datatype (Data::CtrlParam_DataType_INT);
              ctrlParamData.set_val0 (paramXmlNode.attribute ("value").as_int ());
            }
          else if (value == "DBL")
            {
              ctrlParamData.set_datatype (Data::CtrlParam_DataType_DBL);
              ctrlParamData.set_val1 (paramXmlNode.attribute ("value").as_double ());
            }
          else if (value == "STR")
            {
              ctrlParamData.set_datatype (Data::CtrlParam_DataType_STR);
              ctrlParamData.set_val2 (paramXmlNode.attribute ("value").value ());
            }
          else if (value == "BOOL")
            {
              ctrlParamData.set_datatype (Data::CtrlParam_DataType_BOOL);
              ctrlParamData.set_val3 (paramXmlNode.attribute ("value").as_bool ());
            }
          else
            {
              internalStatus = "Unknown <data_type> value in CtrlParam.";
              return false;
            }
        }

      value = "";
      value = paramXmlNode.attribute ("units").value ();
      ctrlParamData.set_units (value);

      value = "";
      value = paramXmlNode.attribute ("descr").value ();
      ctrlParamData.set_descr (value);

      value = "";
      value = paramXmlNode.attribute ("set").value ();
      if (value.size () > 0)
          ctrlParamData.set_isready (paramXmlNode.attribute ("set").as_bool ());

      value = "";
      value = paramXmlNode.attribute ("required").value ();
      if (value.size () > 0)
          ctrlParamData.set_isrequired (paramXmlNode.attribute ("required").as_bool ());

      value = "";
      value = paramXmlNode.attribute ("modifiable").value ();
      if (value.size () > 0)
          ctrlParamData.set_ismodifiable (paramXmlNode.attribute ("modifiable").as_bool ());

      value = "";
      value = paramXmlNode.attribute ("savable").value ();
      if (value.size () > 0)
          ctrlParamData.set_issavable (paramXmlNode.attribute ("savable").as_bool ());

      value = "";
      value = paramXmlNode.attribute ("multiple").value ();
      if (value.size () > 0)
          ctrlParamData.set_ismultiple (paramXmlNode.attribute ("multiple").as_bool ());

      /// Parsing key-value pairs if the CtrlParam had multiple possible values
      if (ctrlParamData.ismultiple ())
        {
          for (pugi::xml_node keyValuePairXmlNode = paramXmlNode.first_child ();
              keyValuePairXmlNode; keyValuePairXmlNode = keyValuePairXmlNode.next_sibling ())
            {
              value = "";
              value = keyValuePairXmlNode.attribute ("name").value ();
              if (value.size () > 0)
                {
                  ctrlParamData.add_keyname (value);
                }
              else
                {
                  internalStatus = "Missing <key> attribute in key-value of a CtrlParam.";
                  return false;
                }

              value = "";
              value = keyValuePairXmlNode.attribute ("value").value ();
              if (value.size () > 0)
                {
                  switch (ctrlParamData.datatype ())
                  {
                  case Data::CtrlParam_DataType_INT :
                    {
                      ctrlParamData.add_val0list (
                          keyValuePairXmlNode.attribute ("value").as_int ());
                      break;
                    }
                  case Data::CtrlParam_DataType_DBL :
                    {
                      ctrlParamData.add_val1list (
                          keyValuePairXmlNode.attribute ("value").as_double ());
                      break;
                    }
                  case Data::CtrlParam_DataType_STR :
                    {
                      ctrlParamData.add_val2list (value);
                      break;
                    }
                  case Data::CtrlParam_DataType_BOOL :
                    {
                      ctrlParamData.add_val3list (
                          keyValuePairXmlNode.attribute ("value").as_bool ());
                      break;
                    }
                  }
                }
              else
                {
                  internalStatus = "Missing <value> attribute in key-value of a CtrlParam.";
                  return false;
                }

              ++tmpCount;
            }

          if (tmpCount == 0)
            {
              internalStatus = "No <key-value> pairs parsed as expected for a CtrlParam.";
              return false;
            }
        }

      ctrlParamList->push_back (ctrlParamData);
      ++(*ctrlParamId);
      ++paramCount;
    }

  /// If the expected parameter count is not the same as parsed from service
  if (paramCount != expected)
    {
      internalStatus = "Mismatch in CtrlParam count.";
      return false;
    }

  return true;
}

bool
XmlServiceParser::parseResources (pugi::xml_node resources, unsigned int serviceId,
    const std::string& globalServiceId, unsigned int expected)
{
  /// TODO
}
