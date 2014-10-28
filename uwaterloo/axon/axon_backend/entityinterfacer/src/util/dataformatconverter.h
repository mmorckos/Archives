/*
 * dataformatter.h
 *
 *  Created on: 2011-12-05
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef DATAFORMATCONVERTER_H_
#define DATAFORMATCONVERTER_H_

#if defined (__linux__)
#include "../protobufdata/param.pb.h"
#include "../protobufdata/ctrlparam.pb.h"
#include "../protobufdata/resource.pb.h"
#include "../protobufdata/service.pb.h"
#include "../protobufdata/entityprofile.pb.h"
#include "../protobufdata/task.pb.h"
#include "../protobufdata/position.pb.h"
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
  /*! \brief Constructor.
   */
  DataFormatConverter();

  /*! \brief Destructor.
   */
  virtual ~DataFormatConverter();

  /*! \brief This function converts a BasicProfile data item to string format and vice versa.
   *
   * The conversion is made for data exchange over the network.
   *
   * \param profile of type Data::BasicProfile* which is the BasicProfile data item to be
   * created from or converted to a string.
   * \param msg of type string* which is the string to be converted to or created from
   * the BasicProfile data item.
   * \param flag of type bool which controls the direction of conversion.
   * \return true if conversion was successful. false otherwise.
   */
  bool formatBasicProfile (Data::BasicProfile *profile, std::string *msg, const bool flag);

  /*! \brief This function converts a Service data item to string format and vice versa.
   *
   * The conversion is made for data exchange over the network.
   *
   * \param service of type Data::Service* which is the Service data item to be created from or
   * converted to a string.
   * \param msg of type string* which is the string to be converted to or created from the Service
   * data item.
   * \param flag of type bool which controls the direction of conversion.
   * \return true if conversion was successful. false otherwise.
   */
  bool formatService (Data::Service *service, std::string *msg, const bool flag,
      const std::string& prefix = SVC);
  bool formatService (Data::Service *service, char *msg, const size_t size, const bool flag,
      const std::string& prefix = SVC);

  /*! \brief This function converts a CtrlParam data item to string format and vice versa.
   *
   * The conversion is made for data exchange over the network.
   *
   * \param ctrlParam of type Data::CtrlParam* which is the CtrlParam data item to be created from or
   * converted to a string.
   * \param msg of type string* which is the string to be converted to or created from the CtrlParam
   * data item.
   * \param flag of type bool which controls the direction of conversion.
   * \return true if conversion was successful. false otherwise.
   */
  bool formatCtrlParam (Data::CtrlParam *ctrlParam, std::string *msg, const bool flag,
      const std::string& prefix = CTRLPARAM);
  bool formatCtrlParam (Data::CtrlParam *ctrlParam, char *msg, const size_t size,
      const bool flag, const std::string& prefix = CTRLPARAM);

  /*! \brief This function converts a Param data item to string format and vice versa.
   *
   * The conversion is made for data exchange over the network.
   *
   * \param param of type Data::Param* which is the Param data item to be created from or
   * converted to a string.
   * \param msg of type string* which is the string to be converted to or created from the Param
   * data item.
   * \param flag of type bool which controls the direction of conversion.
   * \return true if conversion was successful. false otherwise.
   */
  bool formatParam (Data::Param *param, std::string *msg, const bool flag,
      const std::string& prefix = PARAM);
  bool formatParam (Data::Param *param, char *msg, const size_t size, const bool flag,
      const std::string& prefix = PARAM);

  /*! \brief This function converts a Resource data item to string format and vice versa.
   *
   * The conversion is made for data exchange over the network.
   *
   * \param resource of type Data::Resource* which is the Resource data item to be created from or
   * converted to a string.
   * \param msg of type string* which is the string to be converted to or created from the Resource
   * data item.
   * \param flag of type bool which controls the direction of conversion.
   * \return true if conversion was successful. false otherwise.
   */
  bool formatResource (Data::Resource *resource, std::string *msg, const bool flag,
      const std::string& prefix = RESOURCE);
  bool formatResource (Data::Resource *resource, char *msg, const size_t size,
      const bool flag, const std::string& prefix = RESOURCE);

  /*! \brief This function converts a Task to string format and vice versa.
   *
   * The conversion is made for data exchange over the network.
   *
   * \param task of type Data::Task* which is the Task data item to be created from or
   * converted to a string.
   * \param msg of type string* which is the string to be converted to or created from the Task
   * data item.
   * \param flag of type bool which controls the direction of conversion.
   * \return true if conversion was successful. false otherwise.
   */
  bool formatTask (Data::Task *task, std::string *msg, const bool flag,
      const std::string& prefix = TSK);
  bool formatTask (Data::Task *task, char *msg, const size_t size, const bool flag,
      const std::string& prefix = TSK);

  /*! \brief This function converts a Position to string format and vice versa.
   *
   * The conversion is made for data exchange over the network.
   *
   * \param task of type Data::Position* which is the Task data item to be created from or
   * converted to a string.
   * \param msg of type string* which is the string to be converted to or created from the Position
   * data item.
   * \param flag of type bool which controls the direction of conversion.
   * \return true if conversion was successful. false otherwise.
   */
  bool formatPosition (Data::Position *position, std::string *msg, const bool flag,
      const std::string& prefix = POS);
  bool formatPosition (Data::Position *position, char *msg, const size_t size,
      const bool flag, const std::string& prefix = POS);

  /*! \brief This function extracts the prefix of a message.
   *
   * \param msg Pointer to message of type string.
   * \param prefix Pointer to message prefix of type string.
   * \return true if message is properly formatted. false otherwise.
   */
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

  /*! \brief This function removes the identification prefix from a message.
   *
   * This is done in preparation for creating a data item out of the message.
   *
   * \param msg of type string*
   * \return true if the message is properly formatted. false otherwise.
   */
  bool getBody (std::string *msg, std::string *body);
  int getBody (char *msg, const size_t size0, char *body, const size_t size1);
};

#endif /* DATAFORMATCONVERTER_H_ */
