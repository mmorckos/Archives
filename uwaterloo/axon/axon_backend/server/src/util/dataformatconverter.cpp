/*
 * dataformatter.cpp
 *
 *  Created on: 2011-12-05
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <string.h>
#include <iostream>

#include "dataformatconverter.h"
#include "../common/status.h"

/*! \brief Constructor.
 */
DataFormatConverter::DataFormatConverter()
{}

/*! \brief Destructor.
 */
DataFormatConverter::~DataFormatConverter()
{}

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
/*
bool
DataFormatConverter::formatBasicProfile (Data::BasicProfile *profile, std::string *msg,
    const bool flag)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!profile->IsInitialized ())
            {
              internalStatus = "One or more required fields in <BasicProfile> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (PRFL);
          msg->append (DELIM);
          profile->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          profile->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& e)
    {
      internalStatus = flag ? "Failed to serialize <BasicProfile> due to an exception." :
                              "Failed to <BasicProfile> due to an exception.";
      return false;
    }

  return true;
}
*/

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
/*
bool
DataFormatConverter::formatService (Data::Service *service, std::string *msg, const bool flag,
    const std::string& prefix)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!service->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Service> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (prefix);
          msg->append (DELIM);
          service->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          service->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Service> due to an exception." :
                              "Failed to parse <Service> due to an exception.";
      return false;
    }

  return true;
}
*/

/*
bool
DataFormatConverter::formatService (Data::Service *service, char *msg, const size_t size,
    const bool flag, const std::string& prefix)
{
  char tmp[size];
  char body[size];
  unsigned int i;

  msg[0] = '\0';

  try
    {
      if (flag)
        {
          if (!service->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Service> are not set.";
              return false;
            }

          service->SerializeToArray (tmp, size);
          for (i = 0; i < prefix.size (); ++i)
              msg[i] = prefix[i];

          msg[i++] = DELIM[0];

          for (unsigned int j = 0, k = i; j < size && k < size; ++j, ++k)
              msg[k] = tmp[j];
        }
      else
        {
          i = getBody (msg, size, body, size);
          if (i == ERR)
              return false;

          service->ParseFromArray (body, i);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Service> due to an exception." :
                              "Failed to parse <Service> due to an exception.";
      return false;
    }

  return true;
}
*/

/*! \brief This function converts a CtrlParam data item to string format and vice versa.
 *
 * The conversion is made for data exchange over the network.
 *
 * \param service of type Data::CtrlParam* which is the CtrlParam data item to be created from or
 * converted to a string.
 * \param msg of type string* which is the string to be converted to or created from the CtrlParam
 * data item.
 * \param flag of type bool which controls the direction of conversion.
 * \return true if conversion was successful. false otherwise.
 */
/*
bool
DataFormatConverter::formatCtrlParam (Data::CtrlParam *ctrlParam, std::string *msg, const bool flag,
    const std::string& prefix)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!ctrlParam->IsInitialized ())
            {
              internalStatus = "One or more required fields in <CtrlParam> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (prefix);
          msg->append (DELIM);
          ctrlParam->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          ctrlParam->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <CtrlParam> due to an exception." :
                              "Failed to parse <CtrlParam> due to an exception.";
      return false;
    }

  return true;
}
*/

/*
bool
DataFormatConverter::formatCtrlParam (Data::CtrlParam *ctrlParam, char *msg, const size_t size,
    const bool flag, const std::string& prefix)
{
  char tmp[size];
  char body[size];
  unsigned int i;

  msg[0] = '\0';

  try
    {
      if (flag)
        {
          if (!ctrlParam->IsInitialized ())
            {
              internalStatus = "One or more required fields in <CtrlParam> are not set.";
              return false;
            }

          ctrlParam->SerializeToArray (tmp, size);
          for (i = 0; i < prefix.size (); ++i)
              msg[i] = prefix[i];

          msg[i++] = DELIM[0];

          for (unsigned int j = 0, k = i; j < size && k < size; ++j, ++k)
              msg[k] = tmp[j];
        }
      else
        {
          i = getBody (msg, size, body, size);
          if (i == ERR)
              return false;

          ctrlParam->ParseFromArray (body, i);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <CtrlParam> due to an exception." :
                              "Failed to parse <CtrlParam> due to an exception.";
      return false;
    }

  return true;
}
*/

/*! \brief This function converts a Param data item to string format and vice versa.
 *
 * The conversion is made for data exchange over the network.
 *
 * \param service of type Data::Param* which is the Param data item to be created from or
 * converted to a string.
 * \param msg of type string* which is the string to be converted to or created from the Param
 * data item.
 * \param flag of type bool which controls the direction of conversion.
 * \return true if conversion was successful. false otherwise.
 */
/*
bool
DataFormatConverter::formatParam (Data::Param *param, std::string *msg, const bool flag,
    const std::string& prefix)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!param->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Param> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (prefix);
          msg->append (DELIM);
          param->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          param->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Param> due to an exception." :
                              "Failed to parse <Param> due to an exception.";
      return false;
    }

  return true;
}
*/

/*
bool
DataFormatConverter::formatParam (Data::Param *param, char *msg, const size_t size,
    const bool flag, const std::string& prefix)
{
  char tmp[size];
  char body[size];
  unsigned int i;

  msg[0] = '\0';

  try
    {
      if (flag)
        {
          if (!param->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Param> are not set.";
              return false;
            }

          param->SerializeToArray (tmp, size);
          for (i = 0; i < prefix.size (); ++i)
              msg[i] = prefix[i];

          msg[i++] = DELIM[0];

          for (unsigned int j = 0, k = i; j < size && k < size; ++j, ++k)
              msg[k] = tmp[j];
        }
      else
        {
          i = getBody (msg, size, body, size);
          if (i == ERR)
              return false;

          param->ParseFromArray (body, i);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Param> due to an exception." :
                              "Failed to parse <Param> due to an exception.";
      return false;
    }

  return true;
}
*/

/*! \brief This function converts a Resource data item to string format and vice versa.
 *
 * The conversion is made for data exchange over the network.
 *
 * \param service of type Data::Resource* which is the Resource data item to be created from or
 * converted to a string.
 * \param msg of type string* which is the string to be converted to or created from the Resource
 * data item.
 * \param flag of type bool which controls the direction of conversion.
 * \return true if conversion was successful. false otherwise.
 */
/*
bool
DataFormatConverter::formatResource (Data::Resource *resource, std::string *msg, const bool flag,
    const std::string& prefix)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!resource->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Resource> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (prefix);
          msg->append (DELIM);
          resource->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          resource->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Resource> due to an exception." :
                              "Failed to parse <Resource> due to an exception.";
      return false;
    }

  return true;
}
*/
/*
bool
DataFormatConverter::formatResource (Data::Resource *resource, char *msg, const size_t size,
    const bool flag, const std::string& prefix)
{
  char tmp[size];
  char body[size];
  unsigned int i;

  msg[0] = '\0';

  try
    {
      if (flag)
        {
          if (!resource->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Resource> are not set.";
              return false;
            }

          resource->SerializeToArray (tmp, size);
          for (i = 0; i < prefix.size (); ++i)
              msg[i] = prefix[i];

          msg[i++] = DELIM[0];

          for (unsigned int j = 0, k = i; j < size && k < size; ++j, ++k)
              msg[k] = tmp[j];
        }
      else
        {
          i = getBody (msg, size, body, size);
          if (i == ERR)
              return false;

          resource->ParseFromArray (body, i);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Resource> due to an exception." :
                              "Failed to parse <Resource> due to an exception.";
      return false;
    }

  return true;
}
*/

/*
bool
DataFormatConverter::formatTask (Data::Task *task, std::string *msg, const bool flag,
    const std::string& prefix)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!task->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Task> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (prefix);
          msg->append (DELIM);
          task->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          task->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Task> due to an exception." :
                              "Failed to parse <Task> due to an exception.";
      return false;
    }

  return true;
}

/*
bool
DataFormatConverter::formatTask (Data::Task *task, char *msg, const size_t size,
    const bool flag, const std::string& prefix)
{
  char tmp[size];
  char body[size];
  unsigned int i;

  msg[0] = '\0';

  try
    {
      if (flag)
        {
          if (!task->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Task> are not set.";
              return false;
            }

          task->SerializeToArray (tmp, size);
          for (i = 0; i < prefix.size (); ++i)
              msg[i] = prefix[i];

          msg[i++] = DELIM[0];

          for (unsigned int j = 0, k = i; j < size && k < size; ++j, ++k)
              msg[k] = tmp[j];
        }
      else
        {
          i = getBody (msg, size, body, size);
          if (i == ERR)
              return false;

          task->ParseFromArray (body, i);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Task> due to an exception." :
                              "Failed to parse <Task> due to an exception.";
      return false;
    }

  return true;
}
*/

/*
bool
DataFormatConverter::formatPosition (Data::Position *position, std::string *msg, const bool flag,
    const std::string& prefix)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!position->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Position> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (prefix);
          msg->append (DELIM);
          position->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          position->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Position> due to an exception." :
                              "Failed to parse <Position> due to an exception.";
      return false;
    }

  return true;
}
*/

/*
bool
DataFormatConverter::formatPosition (Data::Position *position, char *msg, const size_t size,
    const bool flag, const std::string& prefix)
{
  char tmp[size];
  char body[size];
  unsigned int i;

  msg[0] = '\0';

  try
    {
      if (flag)
        {
          if (!position->IsInitialized ())
            {
              internalStatus = "One or more required fields in <Position> are not set.";
              return false;
            }

          position->SerializeToArray (tmp, size);
          for (i = 0; i < prefix.size (); ++i)
              msg[i] = prefix[i];

          msg[i++] = DELIM[0];

          for (unsigned int j = 0, k = i; j < size && k < size; ++j, ++k)
              msg[k] = tmp[j];
        }
      else
        {
          i = getBody (msg, size, body, size);
          if (i == ERR)
              return false;

          position->ParseFromArray (body, i);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <Position> due to an exception." :
                              "Failed to parse <Position> due to an exception.";
      return false;
    }

  return true;
}
*/

bool
DataFormatConverter::formatGlobalService (Data::GlobalService *globalService, std::string *msg,
    const bool flag, const std::string& prefix)
{
  std::string tmp;
  std::string body;

  try
    {
      if (flag)
        {
          if (!globalService->IsInitialized ())
            {
              internalStatus = "One or more required fields in <GlobalService> are not set.";
              return false;
            }

          (*msg) = "";
          msg->append (prefix);
          msg->append (DELIM);
          globalService->SerializeToString (&tmp);
          msg->append (tmp);
        }
      else
        {
          if (!getBody (msg, &body))
              return false;

          globalService->ParseFromString (body);
        }
    }
  catch (google::protobuf::FatalException& ex)
    {
      internalStatus = flag ? "Failed to serialize <GlobalService> due to an exception." :
                              "Failed to parse <Position> due to an exception.";
      return false;
    }

  return true;
}

bool
DataFormatConverter::getPrefix (std::string *msg, std::string *prefix)
{
  size_t pos;

  (*prefix) = "";
  pos = msg->find (DELIM);

  if (pos == std::string::npos)
    {
      internalStatus = "Message is not properly formatted.";
      return false;
    }

  (*prefix) = msg->substr (0, pos);

  return true;
}

bool
DataFormatConverter::getPrefix (char *msg, const size_t size, std::string *prefix)
{
  size_t pos;
  unsigned int j;

  pos = 0;
  (*prefix) = "";

  for (unsigned int i = 0; i < size; ++i)
    {
      if (msg[i] == DELIM[0])
          break;

      prefix->append (1, msg[i]);
      ++pos;
    }

  if (pos == 0 || pos == size)
    {
      internalStatus = "Message is not properly formatted.";
      return false;
    }

  return true;
}

/*! \brief This function removes the identification prefix from a message.
 *
 * This is done in preparation for creating a data item out of the message.
 *
 * \param msg of type string*
 * \return true if the message is properly formatted. false otherwise.
 */
bool
DataFormatConverter::getBody (std::string *msg, std::string *body)
{
  size_t pos;

  pos = msg->find (DELIM);

  if (pos == std::string::npos)
    {
      internalStatus = "Message is not properly formatted.";
      return false;
    }

  (*body) = msg->substr (pos + 1);

  return true;
}

int
DataFormatConverter::getBody (char *msg, const size_t size0, char *body,
    const size_t size1)
{
  size_t pos;
  unsigned int j;

  pos = 0;

  for (unsigned int i = 0; i < size0; ++i)
    {
      if (msg[i] == DELIM[0])
          break;

      ++pos;
    }

  if (pos == 0 || pos == size0)
    {
      internalStatus = "Message is not properly formatted.";
      return -1;
    }

  for (unsigned int i = pos + 1, j = 0; i < size0 && j < size1; ++i, ++j)
      body[j] = msg[i];

  if (j == size1)
    {
      internalStatus = "Size of buffer is too small.";
      return -1;
    }

  body[size0] = '\0';

  return (size0 - (pos + 1));
}
