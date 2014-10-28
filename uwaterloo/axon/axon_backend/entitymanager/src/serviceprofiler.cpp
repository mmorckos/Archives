/*
 * serviceprofiler.cpp
 *
 *  Created on: 2011-12-20
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <utility>

#include "serviceprofiler.h"
#if defined (__linux__)
#include "./common/global.h"
#include "./common/status.h"
#include "./util/configparser.h"
//#include "./util/serviceparser.h"
#include "./util/xmlserviceparser.h"
#elif defined (__WIN32)
#include "..\\util\\serviceparser.h"
#include "..\\common\\common.h"
#endif

#include "./util/dataformatconverter.h"

ServiceProfiler::ServiceProfiler (const Ice::LoggerPtr& logger, const bool& flag) :
  GENERAL_CFG ("../config/general.cfg"),
  GENERAL_CFG_COUNT (4),
  FIELD_NAME_CC (0),
  FIELD_TYPE_CC (0),
  FIELD_DESCR_CC (0),
  FIELD_NOTES_CC (0),
  iceLogger_Ptr (logger),
  logFlag (flag)
{}

ServiceProfiler::~ServiceProfiler ()
{}

bool
ServiceProfiler::init ()
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
          oss << "ServiceProfiler: " << "failed to load general configurations.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

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
      else if ((*itr).first == "NOTESCC")
        {
          FIELD_NOTES_CC = atoi ((*itr).second.c_str ());
          ++count;
        }
      else
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "unknown parameters in general configurations.";
              iceLogger_Ptr->trace ("Warning", oss.str ());
            }
        }
    }

  if (count != GENERAL_CFG_COUNT)
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "ServiceProfiler: ";
          oss << "missing or extraneous parameters in general configurations.";
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

  return true;
}

bool
ServiceProfiler::load (const std::string& fileName)
{
  XmlServiceParser parser;
  std::ostringstream oss;

  parser.init (&serviceList, &ctrlParamList, &resourceList);
  if (!parser.setFile (fileName) || !parser.parse ())
    {
      if (logFlag)
        {
          oss << "ServiceProfiler: " << parser.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return false;
    }

  setEntityId ("NONE");

  return setDependencies ();
}

bool
ServiceProfiler::verify ()
{
  DataFormatConverter ft;
  std::string tmp;
  std::ostringstream oss;
  std::map <std::string, std::string> dupCheck0;
  std::map <std::string, std::string> dupCheck1;
  int currServiceId;

  currServiceId = 0;

  /// Verifying Service
  for (unsigned int i = 0; i < serviceList.size (); ++i)
    {
      Data::Service s;

      if (serviceList[i].name ().size () > FIELD_NAME_CC)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "length of <NAME> in Service '" << i << "' ";
              oss << "is greater than max length.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      else if (serviceList[i].descr ().size () > FIELD_DESCR_CC)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "length of <DESCR> in Service '" << i << "' ";
              oss << "is greater than max length.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      else if (serviceList[i].notes ().size () > FIELD_NOTES_CC)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "length of <NOTES> in Service '" << i << "' ";
              oss << "is greater than max length.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      /// Checking for duplicate names
      if (dupCheck0.find (serviceList[i].name ()) == dupCheck0.end ())
        {
          dupCheck0[serviceList[i].name ()] = "";
        }
      else
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "duplicate Service <name> '";
              oss << serviceList[i].name () << "'";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      /// Checking for duplicate global ids
      if (serviceList[i].globalid ().size () > 0)
        {
          if (dupCheck1.find (serviceList[i].globalid ()) == dupCheck1.end ())
            {
              dupCheck1[serviceList[i].globalid ()] = "";
            }
          else
            {
              if (logFlag)
                {
                  oss.str ("");
                  oss << "ServiceProfiler: " << "duplicate Service <global_id> '";
                  oss << serviceList[i].globalid () << "'.";
                  iceLogger_Ptr->trace ("Error", oss.str ());
                }

              return false;
            }
        }

      if (!ft.formatService (&serviceList[i], &tmp, true) || !ft.formatService (&s, &tmp, false))
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "verification failed: " << ft.getInternalStatus ();
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
    }
  /// Verifying CtrlParam
  dupCheck0.clear ();
  for (unsigned int i = 0; i < ctrlParamList.size (); ++i)
    {
      Data::CtrlParam p;

      if (ctrlParamList[i].serviceid () != currServiceId)
        {
          currServiceId = ctrlParamList[i].serviceid ();
          dupCheck0.clear ();
        }

      if (ctrlParamList[i].name ().size () > FIELD_NAME_CC)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "length of <NAME> in CtrlParam '" << i << "' ";
              oss << "is greater than max length.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      else if (ctrlParamList[i].descr ().size () > FIELD_DESCR_CC)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "length of <descr> in CtrlParam '" << i << "' ";
              oss << "is greater than max length.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      else if (ctrlParamList[i].units ().size () > FIELD_NAME_CC)
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "length of <notes> in CtrlParam '" << i << "' ";
              oss << "is greater than max length.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
      /// Checking for duplicate names within a service
      if (dupCheck0.find (ctrlParamList[i].name ()) == dupCheck0.end ())
        {
          dupCheck0[ctrlParamList[i].name ()] = "";
        }
      else
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "duplicate CtrlParam <name> in Service '";
              oss << serviceList[currServiceId].name () << "'.";
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }

      if (!ft.formatCtrlParam (&ctrlParamList[i], &tmp, true)
          || !ft.formatCtrlParam (&p, &tmp, false))
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "verification failed: " << ft.getInternalStatus ();
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }
    }
  /// Verifying Resource
  for (unsigned int i = 0; i < resourceList.size (); ++i)
    {
      Data::Resource r;

      if (!ft.formatResource (&resourceList[i], &tmp, true) || !ft.formatResource (&r, &tmp, false))
        {
          if (logFlag)
            {
              oss.str ("");
              oss << "ServiceProfiler: " << "verification failed: " << ft.getInternalStatus ();
              iceLogger_Ptr->trace ("Error", oss.str ());
            }

          return false;
        }

      r.ParseFromString (tmp);
    }

  return true;
}

void
ServiceProfiler::setEntityId (const std::string& entityUuid)
{
  for (unsigned int i = 0; i < serviceList.size (); ++i)
      serviceList[i].set_entityid (entityUuid);
}

int
ServiceProfiler::compileServiceRecord (unsigned int id, Util::DataRecord *svcDataRecord)
{
  DataFormatConverter ft;
  std::string tmp;
  std::vector <std::string> serviceData;
  std::vector <std::string> ctrlParamDataList;
  std::vector <std::string> resourceDataList;
  std::ostringstream oss;

  if (!checkServiceId (id))
      return ERR;

  serviceData.push_back (SVC);
  ctrlParamDataList.push_back (CTRLPARAM);
  resourceDataList.push_back (RESOURCE);

  if (!ft.formatService (&serviceList[id], &tmp, true))
    {
      if (logFlag)
        {
          oss.str ("");
          oss << "ServiceProfiler: " << ft.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return FALSE;
    }
  else
    {
      serviceData.push_back (tmp);
    }

  for (unsigned int i = 0; i < ctrlParamList.size (); ++i)
    {
      if (ctrlParamList[i].serviceid () == (int) id)
        {
          if (!ft.formatCtrlParam (&ctrlParamList[i], &tmp, true))
            {
              if (logFlag)
                {
                   oss.str ("");
                   oss << "ServiceProfiler: " << ft.getInternalStatus ();
                   iceLogger_Ptr->trace ("Error", oss.str ());
                }

              return FALSE;
            }
          else
            {
              ctrlParamDataList.push_back (tmp);
            }

          std::cout << std::endl;
        }
    }

  for (unsigned int i = 0; i < resourceList.size (); ++i)
    {
      if (resourceList[i].serviceid () == (int) id)
        {
          if (!ft.formatResource (&resourceList[i], &tmp, true))
            {
              if (logFlag)
                {
                   oss.str ("");
                   oss << "ServiceProfiler: " << ft.getInternalStatus ();
                   iceLogger_Ptr->trace ("Error", oss.str ());
                }

              return FALSE;
            }
          else
            {
              resourceDataList.push_back (tmp);
            }
        }
    }

  svcDataRecord->dataList.push_back (serviceData);
  svcDataRecord->dataList.push_back (ctrlParamDataList);
  svcDataRecord->dataList.push_back (resourceDataList);

  return OK;
}

bool
ServiceProfiler::setDependencies ()
{
  unsigned int depCount;
  unsigned int baseDepCount;
  std::ostringstream oss;

  for (unsigned int i = 0; i < serviceList.size (); ++i)
    {
      depCount = 0;
      baseDepCount = 0;

      for (unsigned j = 0; j < ctrlParamList.size (); ++j)
        {
          if ((ctrlParamList[j].serviceid () == serviceList[i].id ()))
            {
              if (ctrlParamList[j].isrequired() && !ctrlParamList[j].isready ())
                {
                  if (!ctrlParamList[j].issavable ())
                      ++baseDepCount;
                  else
                      ++depCount;
                }

              if (!ctrlParamList[j].isready () && !ctrlParamList[j].ismodifiable ())
                {
                  if (logFlag)
                    {
                       oss.str ("");
                       oss << "ServiceProfiler: " << " CtrlParam '" << ctrlParamList[j].name ();
                       oss << "' cannot be both unset and not modifiable.";
                       iceLogger_Ptr->trace ("Error", oss.str ());
                    }

                  return false;
                }
            }
        }

      for (unsigned int j = 0; j < resourceList.size (); ++j)
        {
          if ((resourceList[j].serviceid () == resourceList[i].id ()))
            {
              if (resourceList[j].isrequired() && !resourceList[j].isready ())
                {
                  if (!resourceList[j].issavable ())
                      ++baseDepCount;
                  else
                      ++depCount;
                }

              if (!resourceList[j].isready () && !resourceList[j].ismodifiable ())
                {
                  if (logFlag)
                    {
                       oss.str ("");
                       oss << "ServiceProfiler: " << " Resource '" << resourceList[j].name ();
                       oss << "' cannot be both unset and not modifiable.";
                       iceLogger_Ptr->trace ("Error", oss.str ());
                    }

                  return false;
                }
            }
        }

      serviceList[i].set_depcount (depCount);
      serviceList[i].set_fixeddepcount (baseDepCount);
      serviceList[i].set_basedepcount (baseDepCount);
    }

  return true;
}

int
ServiceProfiler::updateResource (Data::Resource *resource)
{
  unsigned int resourceId;
  unsigned int serviceId;
  std::ostringstream oss;
//  std::ifstream file;

  resourceId = resource->id ();
  serviceId = resource->serviceid ();

  if (!checkResourceId (resourceId, serviceId))
      return ERR;

  if (!resourceList[resourceId].ismodifiable ())
    {

      if (logFlag)
        {
          oss << "ServiceProfiler: " << "<Resource> is not modifiable";
          iceLogger_Ptr->trace ("Warning", oss.str ());
        }

      return FALSE;
    }

  /// TODO
/*
  try
  {
    file.open (resource->url ().c_str ());
  }
  catch (std::ifstream::failure e)
  {
    internalStatus = "ERROR! Unable to check resource file.";
    return false;
  }

  if (!file.is_open ())
    {
      internalStatus = "ERROR! Invalid Resource URL or Resource file does not exists.";
      return false;
    }

  file.close ();
*/
  resourceList[resourceId].set_url (resource->url ());
  resourceList[resourceId].set_isready (true);
  if (serviceList[serviceId].depcount () > 0)
      serviceList[serviceId].set_depcount (serviceList[serviceId].depcount () - 1);

  return OK;
}

int
ServiceProfiler::updateResource (std::string resourceData)
{
  DataFormatConverter ft;
  Data::Resource resource;
  std::ostringstream oss;

  if (!ft.formatResource(&resource, &resourceData, false))
    {
      if (logFlag)
        {
          oss << "ServiceProfiler: " << ft.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return ERR;
    }
  else
    {
      return updateResource (&resource);
    }
}

int
ServiceProfiler::updateCtrlParam (Data::Param *param)
{
  unsigned int id;
  unsigned int ctrlParamId;
  unsigned int serviceId;
  std::ostringstream oss;

  id = param->id ();
  ctrlParamId = param->ctrlparamid ();
  serviceId = param->serviceid ();

  if (!checkParamId (id, ctrlParamId, serviceId))
      return ERR;

  if (!ctrlParamList[ctrlParamId].ismodifiable ())
    {
      if (logFlag)
        {
          oss << "ServiceProfiler: " << "<CtrlParam> is not modifiable";
          iceLogger_Ptr->trace ("Warning", oss.str ());
        }

      return FALSE;
    }

  if (param->datatype() == param->INT)
      ctrlParamList[id].set_val0 (param->val0 ());
  else if (param->datatype() == param->DBL)
      ctrlParamList[id].set_val1 (param->val1 ());
  else if (param->datatype() == param->STR)
      ctrlParamList[id].set_val2 (param->val2 ());
  else
      ctrlParamList[id].set_val3 (param->val3 ());

  ctrlParamList[ctrlParamId].set_isready (true);

  if (!ctrlParamList[ctrlParamId].issavable ())
    {
      if (serviceList[serviceId].fixeddepcount () > 0)
        {
          serviceList[serviceId].set_fixeddepcount (serviceList[serviceId].fixeddepcount () - 1);
        }
    }
  else if (serviceList[serviceId].depcount () > 0)
    {
      serviceList[serviceId].set_depcount (serviceList[serviceId].depcount () - 1);
    }

  return OK;
}

int
ServiceProfiler::updateCtrlParam (std::string paramData)
{
  DataFormatConverter ft;
  Data::Param param;
  std::ostringstream oss;

  if (!ft.formatParam (&param, &paramData, false))
    {
      if (logFlag)
        {
          oss << "ServiceProfiler: " << ft.getInternalStatus ();
          iceLogger_Ptr->trace ("Error", oss.str ());
        }

      return ERR;
    }
  else
    {
      return updateCtrlParam (&param);
    }
}

int
ServiceProfiler::checkAllSet (unsigned int id)
{
  if (!checkServiceId (id))
      return ERR;

  return (int) (serviceList[id].depcount () == 0 && serviceList[id].fixeddepcount () == 0);
}

std::string
ServiceProfiler::getServiceName (unsigned int id)
{
  if (!checkServiceId (id))
      return "";

  return serviceList[id].name ();
}

int
ServiceProfiler::toggleService (unsigned int id, bool flag)
{
  if (!checkServiceId (id))
      return ERR;

  serviceList[id].set_isactive (flag);

  return OK;
}

int
ServiceProfiler::resetServiceDependency (unsigned int id)
{
  if (!checkServiceId (id))
      return ERR;

  serviceList[id].set_fixeddepcount (serviceList[id].basedepcount ());

  return OK;
}

int
ServiceProfiler::getServiceActive (unsigned int id)
{
  if (!checkServiceId (id))
      return ERR;

  return (int) serviceList[id].isactive ();
}

int
ServiceProfiler::setServiceExec (unsigned int id, bool flag)
{
  if (!checkServiceId (id))
      return ERR;

  serviceList[id].set_isexec (flag);

  return OK;
}

int
ServiceProfiler::getServiceExec (unsigned int id)
{
  if (!checkServiceId (id))
      return ERR;

  return (int) serviceList[id].isexec ();
}

unsigned int
ServiceProfiler::getNumOfSvc ()
{
  return serviceList.size ();
}

void
ServiceProfiler::clear ()
{
  serviceList.clear ();
  ctrlParamList.clear ();
  resourceList.clear ();
}
