/*
 * serviceparser.cpp
 *
 *  Created on: 2011-11-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 *
 * DESFUNCT
 */

#include "serviceparser.h"

ServiceParser::ServiceParser ():
  count (0),
  expectedCtrlParamCount (0),
  expectedResourceCount (0),
  ctrlParamCount(0),
  resourceCount (0),
  serviceId (0),
  ctrlParamId (0),
  resourceId (0)
{}

void
ServiceParser::init (std::vector <Data::Service> *l0, std::vector <Data::CtrlParam> *l1,
    std::vector <Data::Resource> *l2)
{
  serviceList = l0;
  ctrlParamList = l1;
  resourceList = l2;
}

/*! \brief Parses the given serviceList file and extracts all service information.
 *
 * \return true if file has a valid format. false otherwise.
 */
bool
ServiceParser::parse ()
{
  assert (file.is_open ());

  std::string line;
  std::string token;
  std::string value;
  int sCount = 0;

  while (!file.eof ())
    {
      getline (file, line);
      ++lineNum;

      if (line [0] == commentChar || line [0] == '\0')
          continue;

      if (line == "SERVICE")
        {
          if (ctrlParamCount != expectedCtrlParamCount)
            {
              internalStatus = "Missing <CtrlParam>s for a <Service>.";
              return false;
            }

          if (resourceCount != expectedResourceCount)
            {
              internalStatus = "Missing <Resources>s for a <Service>.";
              return false;
            }

          expectedCtrlParamCount = 0;
          expectedResourceCount = 0;
          ctrlParamCount = 0;
          resourceCount = 0;

          if (!parse (0))
            {
              return false;
            }
          else
            {
              ++sCount;
              ++serviceId;
            }
        }
      else if (line == "CTRLPARAM")
        {
          ++ctrlParamCount;

          if (ctrlParamCount > expectedCtrlParamCount)
            {
              internalStatus = "Unexpected <CtrlParam> with no owning <Service>.";
              return false;
            }
          else if (!parse (1))
            {
              return false;
            }
          else
            {
 //             ++paramId;
              ++ctrlParamId;
            }
        }
      else if (line == "RESOURCE")
        {
          ++resourceCount;

          if (resourceCount > expectedResourceCount)
            {
              internalStatus = "Unexpected <Resource> with no owning <Service>.";
              return false;
            }
          else if (!parse (2))
            {
              return false;
            }
          else
            {
              ++resourceId;
            }
        }
    }

  if (file.eof () && !eofCheck ())
      return false;

  if (sCount < MINSVC_COUNT)
    {
      internalStatus = "No serviceList parsed.";
      return false;
    }
  else
    {
      setSuccMsg ();
    }

  return true;
}

bool
ServiceParser::parse (unsigned int key)
{
  std::string line;
  size_t index;
  Data::Service service;
  Data::CtrlParam ctrlParam;
  Data::Resource resource;

  count = 0;

  if (key == 0)
    {
      service.set_id (serviceId);
    }
  else if (key == 1)
    {
      ctrlParam.set_id (ctrlParamId);
      ctrlParam.set_serviceid  (serviceId - 1);
    }
  else if (key == 2)
    {
      resource.set_serviceid (serviceId - 1);
      resource.set_id (resourceId);
    }

  while (!file.eof ())
    {
      ++lineNum;
      token = "";
      value = "";

      getline (file, line);

      if (line [0] == commentChar)
          continue;
      else if (line [0] == '\0')
          break;

      index = line.find_first_of (" \t");
      token = line.substr (0, index);

      if (token.find (commentChar) != std::string::npos)
        {
          setErrMsg ();
          return false;
        }

      while (line [++index] == ' ' || line [++index] == '\t')
          ;

      if (line [index - 1] == '"')
        {
          size_t i;
          for (i = index; i < line.size (); ++i)
            {
              if (line [i] == '"')
                {
                  value = line.substr (index, i - index);
                  break;
                }
            }

          if (value.size () == 0)
            {
              setErrMsg ();
              return false;
            }
        }
      else
        {
          value = line.substr (--index, line.size ());
          value = value.substr (0, value.find_first_of (" \t"));
        }

      if (value.find (commentChar) != std::string::npos)
        {
          setErrMsg ();
          return false;
        }

      if (key == 0)
        {
          if (!packService (&service))
              return false;
        }
      else if (key == 1)
        {
          if (!packCtrlParam (&ctrlParam))
              return false;
        }
      else if (key == 2)
        {
          if (!packResource (&resource))
              return false;
        }
    }

  if (key == 0)
    {
      if (count < MIN_SVC_TOKEN_COUNT)
        {
          internalStatus = "Required <Service> tokens are missing.";
          return false;
        }
      else
        {
          serviceList->push_back (service);
        }
    }
  else if (key == 1)
    {
      if (count < MIN_CTRLPARAM_TOKEN_COUNT)
        {
          internalStatus = "Required <CtrlParam> tokens are missing.";
          return false;
        }
      else
        {
          ctrlParamList->push_back (ctrlParam);
        }
    }
  else if (key == 2)
    {
      if (count < MIN_RESOURCE_TOKEN_COUNT)
        {
          internalStatus = "Required <Resource> tokens are missing.";
          return false;
        }
      else
        {
          resourceList->push_back (resource);
        }
    }

  if (file.eof () && !eofCheck ())
      return false;

  return true;
}

bool
ServiceParser::eofCheck ()
{
  if (ctrlParamCount < expectedCtrlParamCount)
    {
      internalStatus = "Missing <CtrlParam>s.";
      return false;
    }

  if (resourceCount < expectedResourceCount)
    {
      internalStatus = "Missing <Resource>s.";
      return false;
    }

  return true;
}

bool
ServiceParser::packService (Data::Service *service)
{
  if (token == "NAME")
    {
      service->set_name (value);
      ++count;
    }
  else if (token == "TYPE")
    {
      service->set_type ((Data::Service_Type) atoi (value.c_str ()));
      ++count;
    }
  else if (token == "ACTIVE")
    {
      if (value == "0")
          service->set_isactive (false);
      else
          service->set_isactive (true);
    }
  else if (token == "DESCR")
    {
      service->set_descr (value);
    }
  else if (token == "NOTES")
    {
      service->set_notes (value);
    }
  else if (token == "BLK")
    {
      if (value == "0")
          service->set_isblocking (false);
      else
          service->set_isblocking (true);
    }
  else if (token == "PAUSB")
    {
      if (value == "0")
          service->set_ispausable (false);
      else
          service->set_ispausable (true);
    }
  else if (token == "SVCLST")
    {
      char str [8];
      const char *tmp =  value.c_str ();
      strncpy (str, tmp, 8);
      char *pch = strtok (str, " ");

      while (pch != NULL)
        {
          service->add_servicelist (atoi (pch));
          pch = strtok (NULL, " ");
        }
    }
  else if (token == "PARAMCOUNT")
    {
      expectedCtrlParamCount = atoi (value.c_str ());
    }
  else if (token == "RESCOUNT")
    {
      expectedResourceCount = atoi (value.c_str ());
    }
  else
    {
      setUnknownErrMsg ("<Service>", token);
      return false;
    }

  return true;
}

bool
ServiceParser::packCtrlParam (Data::CtrlParam *ctrlParam)
{
  if (token == "NAME")
    {
      ctrlParam->set_name (value.c_str ());
      ++count;
    }
  else if (token == "TYPE")
    {
      ctrlParam->set_type ((Data::CtrlParam_Type) atoi (value.c_str ()));
      ++count;
    }
  else if (token == "DTYPE")
    {
      ctrlParam->set_datatype( (Data::CtrlParam_DataType) atoi (value.c_str ()));
      ++count;
    }
  else if (token == "VAL")
    {
      Data::CtrlParam_DataType dataType = ctrlParam->datatype ();

      if (dataType == Data::CtrlParam_DataType_INT)
        {
          ctrlParam->set_val0 (atoi (value.c_str ()));
        }
      else if (dataType == Data::CtrlParam_DataType_DBL)
        {
          ctrlParam->set_val1 (atof (value.c_str ()));
        }
      else if (dataType == Data::CtrlParam_DataType_STR)
        {
          ctrlParam->set_val2 (value.c_str ());
        }
      else if (dataType == Data::CtrlParam_DataType_BOOL)
        {
          if (value == "0")
              ctrlParam->set_val3 (false);
          else
              ctrlParam->set_val3 (true);
        }
      else
        {
          internalStatus = "ERROR! Invalid <Param> data type.";
          return false;
        }
    }
  else if (token == "REQ")
    {
      if (value == "0")
          ctrlParam->set_isrequired (false);
      else
          ctrlParam->set_isrequired (true);
    }
  else if (token == "SET")
    {
      if (value == "0")
          ctrlParam->set_isready (false);
      else
          ctrlParam->set_isready (true);
    }
  else if (token == "MODI")
    {
      if (value == "0")
          ctrlParam->set_ismodifiable (false);
      else
          ctrlParam->set_ismodifiable (true);
    }
  else if (token == "SAVB")
    {
      if (value == "0")
          ctrlParam->set_issavable (false);
      else
          ctrlParam->set_issavable (true);
    }
  else if (token == "UNITS")
    {
      ctrlParam->set_units (value.c_str ());
    }
  else if (token == "DESCR")
    {
      ctrlParam->set_descr (value.c_str ());
    }
/*
  else if (token == "KEYID")
    {
      ctrlParam->set_keyid (atoi (value.c_str ()));
    }
*/
  else
    {
      setUnknownErrMsg ("<CtrlParam> ", token);
      return false;
    }

  return true;
}

bool
ServiceParser::packResource (Data::Resource *resource)
{
  if (token == "NAME")
    {
      resource->set_name (value.c_str ());
      ++count;
    }
  else if (token == "URL")
    {
      resource->set_url (value.c_str ());
      ++count;
    }
  else if (token == "REQ")
    {
      if (value == "0")
          resource->set_isrequired (false);
      else
          resource->set_isrequired (true);
    }
  else if (token == "SET")
    {
      if (value == "0")
          resource->set_isready (false);
      else
          resource->set_isready (true);
    }
  else if (token == "MODI")
    {
      if (value == "0")
          resource->set_ismodifiable (false);
      else
          resource->set_ismodifiable (true);
    }
  else if (token == "SAVB")
    {
      if (value == "0")
          resource->set_issavable (false);
      else
          resource->set_issavable (true);
    }
  else if (token == "DESCR")
    {
      resource->set_descr (value.c_str ());
    }
  else
    {
      setUnknownErrMsg ("<Resource> ", token);
      return false;
    }

  return true;
}
