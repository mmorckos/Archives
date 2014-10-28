/*
 * dataformatter.h
 *
 *  Created on: 2011-12-05
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef DATAFORMATCONVERTER_H_
#define DATAFORMATCONVERTER_H_

#if defined (__linux__)
/*
#include "../protobufdata/param.pb.h"
#include "../protobufdata/ctrlparam.pb.h"
#include "../protobufdata/resource.pb.h"
#include "../protobufdata/service.pb.h"
#include "../protobufdata/entityprofile.pb.h"
#include "../protobufdata/task.pb.h"
#include "../protobufdata/position.pb.h"
*/
#include "../protobufdata/globalservice.pb.h"
#include "../common/global.h"
#elif defined (_WIN32)
#include "..\\structinfo\\service.pb.h"
#include "..\\structinfo\\ctrlparam.pb.h"
#include "..\\structinfo\\param.pb.h"
#include "..\\structinfo\\resource.pb.h"
#endif


class DataFormatConverter
{

public:
  DataFormatConverter();
  virtual ~DataFormatConverter();
/*
  bool formatBasicProfile (Data::BasicProfile *profile, std::string *msg, const bool flag);
  bool formatService (Data::Service *service, std::string *msg, const bool flag,
      const std::string& prefix = SVC);
  bool formatService (Data::Service *service, char *msg, const size_t size, const bool flag,
      const std::string& prefix = SVC);
  bool formatCtrlParam (Data::CtrlParam *ctrlParam, std::string *msg, const bool flag,
      const std::string& prefix = CTRLPARAM);
  bool formatCtrlParam (Data::CtrlParam *ctrlParam, char *msg, const size_t size,
      const bool flag, const std::string& prefix = CTRLPARAM);
  bool formatParam (Data::Param *param, std::string *msg, const bool flag,
      const std::string& prefix = PARAM);
  bool formatParam (Data::Param *param, char *msg, const size_t size, const bool flag,
      const std::string& prefix = PARAM);
  bool formatResource (Data::Resource *resource, std::string *msg, const bool flag,
      const std::string& prefix = RESOURCE);
  bool formatResource (Data::Resource *resource, char *msg, const size_t size,
      const bool flag, const std::string& prefix = RESOURCE);
  bool formatTask (Data::Task *task, std::string *msg, const bool flag,
      const std::string& prefix = TSK);
  bool formatTask (Data::Task *task, char *msg, const size_t size, const bool flag,
      const std::string& prefix = TSK);
  bool formatPosition (Data::Position *position, std::string *msg, const bool flag,
      const std::string& prefix = POS);
  bool formatPosition (Data::Position *position, char *msg, const size_t size,
      const bool flag, const std::string& prefix = POS);
*/
  bool formatGlobalService (Data::GlobalService *globalService, std::string *msg, const bool flag,
      const std::string& prefix = GLOBALSVC);

  bool getPrefix (std::string *msg, std::string *prefix);
  bool getPrefix (char *msg, const size_t size, std::string *prefix);

  /*! \brief Returns the most recent internal status of the data format converter.
   *
   * \return internalStatus of type string.
   */
  std::string getInternalStatus ()
  {
    return internalStatus;
  }

protected:
  std::string internalStatus;   /*!< Status message (for logging purposes) */

  bool getBody (std::string *msg, std::string *body);
  int getBody (char *msg, const size_t size0, char *body, const size_t size1);
};

#endif /* DATAFORMATCONVERTER_H_ */
