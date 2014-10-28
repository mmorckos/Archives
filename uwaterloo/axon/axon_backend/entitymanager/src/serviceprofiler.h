/*
 * serviceprofiler.h
 *
 *  Created on: 2011-12-20
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef SERVICEPROFILER_H_
#define SERVICEPROFILER_H_

#include <vector>
#include <map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#if defined (__linux__)
#include "util/datarecord.h"
#include "./protobufdata/param.pb.h"
#include "./protobufdata/ctrlparam.pb.h"
#include "./protobufdata/resource.pb.h"
#include "./protobufdata/service.pb.h"
#elif defined (__WIN32)
#include "..\\structinfo\\param.pb.h"
#include "..\\structinfo\\ctrlparam.pb.h"
#include "..\\structinfo\\resource.pb.h"
#include "..\\structinfo\\service.pb.h"
#endif

class ServiceProfiler
{

public:
  /*! \brief Constructor.
   *
   * \param logger Reference to the logging object.
   * \param flag Logging flag.
   */
  ServiceProfiler (const Ice::LoggerPtr& logger, const bool& flag);

  /*! \brief Destructor.
   */
  virtual ~ServiceProfiler ();

  /*! \brief Initialization function.
   *
   * Initializes service profiler by loading initial configurations.
   *
   * \param fileName Socket configurations file name.
   *
   * \return true if initialization is success. false otherwise.
   */
  bool init ();

  /*! \brief Services profile loading function.
   *
   * Parses and loads the entity's services profile.
   *
   * \param fileName Services profile.
   *
   * \return true if parsing is success. false otherwise.
   */
  bool load (const std::string& fileName);

//  bool populate (std::vector <std::string> *l0, std::vector <std::string> *l1,
//      std::vector <std::string> *l2);

  /*! \brief Validates that lengths of certain fields are within bounds.
   *
   * \return true if parsing is success. false otherwise.
   */
  bool verify ();

  /*! \brief Sets the entity UUID.
   *
   * \param entityUuid The UUID of the entity.
   */
  void setEntityId (const std::string& entityUuid);

  /*! \brief Packs service data into a data record.
   *
   * \param id Service local ID.
   * \param svcDataRecord Pointer to a data record type.
   *
   * \return Status of the operation.
   */
  int compileServiceRecord (unsigned int id, Util::DataRecord *svcDataRecord);

  /*! \brief Updates stored data for a resource.
   *
   * \param resource Pointer to a resource type.
   *
   * \return Status of the operation.
   */
  int updateResource (Data::Resource *resource);

  /*! \brief Updates stored data for a resource.
   *
   * \param resourceData Serialized resource data.
   *
   * \return Status of the operation.
   */
  int updateResource (std::string resourceData);

  /*! \brief Updates stored data for a parameter.
   *
   * \param resource Pointer to a parameter type.
   *
   * \return Status of the operation.
   */
  int updateCtrlParam (Data::Param *param);

  /*! \brief Updates stored data for a control parameter.
   *
   * \param paramData Serialized control parameter data.
   *
   * \return Status of the operation.
   */
  int updateCtrlParam (std::string paramData);

  /*! \brief Checks that all dependencies of a service are satisfied.
   *
   * \param id Local ID of the service.
   *
   * \return Status of the operation.
   */
  int checkAllSet (unsigned int id);

  /*! \brief Returns the name of a service.
   *
   * \param id Local ID of the service.
   *
   * \return name of the service.
   */
  std::string getServiceName (unsigned int id);

  /*! \brief Activates/deactives a service.
   *
   * \param id Local ID of the service.
   * \param flag Flag specifying the operation.
   *
   * \return Status of the operation.
   */
  int toggleService (unsigned int id, bool flag);

  /*! \brief Resets the dependencies of a service to the initial state.
   *
   * \param id Local ID of the service.
   *
   * \return Status of the operation.
   */
  int resetServiceDependency (unsigned int id);

  /*! \brief Checks whether a service is active or not.
   *
   * \param id Local ID of the service.
   *
   * \return Status of the operation.
   */
  int getServiceActive (unsigned int id);

  /*! \brief Marks a service as executing or not executing.
   *
   * \param id Local ID of the service.
   *
   * \return Status of the operation.
   */
  int setServiceExec (unsigned int id, bool flag);

  /*! \brief Checks whether a service is currentl executing or not.
   *
   * \param id Local ID of the service.
   *
   * \return Status of the operation.
   */
  int getServiceExec (unsigned int id);

  /*! \brief Returns the total number of services in the profile.
   *
   * \return number of services.
   */
  unsigned int getNumOfSvc ();

  /*! \brief Clears the profile.
   */
  void clear ();

protected:
  const std::string GENERAL_CFG;              /*!< General configurations file url. */
  const unsigned int GENERAL_CFG_COUNT;       /*!< Minimum number of configuration parameters. */
  unsigned int FIELD_NAME_CC;                 /*!< Never exceed length for name field. */
  unsigned int FIELD_TYPE_CC;                 /*!< Never exceed length for type field. */
  unsigned int FIELD_DESCR_CC;                /*!< Never exceed length for description field. */
  unsigned int FIELD_NOTES_CC;                /*!< Never exceed length for notes field. */

  std::vector <Data::Service> serviceList;      /*!< Service list. */
  std::vector <Data::CtrlParam> ctrlParamList;  /*!< CtrlParam list. */
  std::vector <Data::Resource> resourceList;    /*!< Resource list. */
  const Ice::LoggerPtr iceLogger_Ptr;           /*!< Ice logging utility */
  const bool logFlag;                           /*!< bool flag to toggle logging */

  /*! \brief Initialize dependencies of all loaded services.
   *
   * \return true if operation is success. false otherwise.
   */
  bool setDependencies ();

  /*! \brief Checks the validity of a given service ID.
   *
   * \return true if the service with given ID exists. false otherwise.
   */
  bool checkServiceId (unsigned int id)
  {
    if (!(id < serviceList.size ()))
      {
        if (logFlag)
          {
            std::ostringstream oss;

            oss << "ServiceProfiler: " << "invalid <Service> ID: " << id;
            iceLogger_Ptr->trace ("Error", oss.str ());
          }

        return false;
      }

    return true;
  }

  /*! \brief Checks the validity of a given resource ID.
   *
   * \return true if the resource with given ID exists. false otherwise.
   */
  bool checkResourceId (unsigned int id, unsigned int serviceId)
  {
    std::ostringstream oss;

    if (!(id < resourceList.size ()))
      {
        if (logFlag)
          {
            oss << "ServiceProfiler: " << "invalid <Resource> ID: " << id;
            iceLogger_Ptr->trace ("Error", oss.str ());
          }

        return false;
      }
    else if (!(serviceId < serviceList.size ()))
      {
        if (logFlag)
          {
            oss << "ServiceProfiler: "
                << "<Resource> is not associated with any registered <Service>.";
            iceLogger_Ptr->trace ("Error", oss.str ());
          }

        return false;
      }

    return true;
  }

  /*! \brief Checks the validity of a given control parameter ID.
   *
   * \return true if the control parameter with given ID exists. false otherwise.
   */
  bool checkParamId (unsigned int id, unsigned int ctrlParamId, unsigned int serviceId)
  {
    std::ostringstream oss;

    if (!(id < ctrlParamList.size ()))
      {
        if (logFlag)
          {
            oss << "ServiceProfiler: " << "invalid <Param> ID: " << id;;
            iceLogger_Ptr->trace ("Error", oss.str ());
          }

        return false;
      }
    else if (!(ctrlParamId < ctrlParamList.size ()))
      {
        if (logFlag)
          {
            oss << "ServiceProfiler: " << "<Param> is not associated with any <CtrlParam>.";
            iceLogger_Ptr->trace ("Error", oss.str ());
          }

        return false;
      }
    else if (!(serviceId < serviceList.size ()))
      {
        if (logFlag)
          {
            oss << "ServiceProfiler: "
                << "<Param> is not associated with any registered <Service>.";
            iceLogger_Ptr->trace ("Error", oss.str ());
          }

        return false;
      }

    return true;
  }
};

#endif /* SERVICEPROFILER_H_ */
