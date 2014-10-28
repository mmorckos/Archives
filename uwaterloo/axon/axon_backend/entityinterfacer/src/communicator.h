/*
 * communicator.h
 *
 *  Created on: 2012-02-06
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_


#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "./protobufdata/task.pb.h"

class EntityInterfacer;

class Communicator : public IceUtil::TimerTask
{

public:
  /*! \brief Constructor.
   *
   * \param ei Pointer to EntityInterfacer object.
   * \param flag Logging flag.
   */
  Communicator (EntityInterfacer *ei, const bool flag);

  /*! \brief Destructor.
   */
  virtual ~Communicator ();

  /*! \brief Main even loop.
   */
  virtual void runTimerTask ();

  /*! \brief Resets the communicator status.
   */
  void reset ();

  /*! \brief Sets expected data type of a data message.
   *
   * \param dataType Expected data type of type string
   */
  void setExpectedDataType (std::string dataType);

  /*! \brief Returns expected data type of a data message.
   *
   * \return Expected data type of type string.
   *
   * \sa setExpectedDataType ()
   */
  std::string getExpectedDataType ();

protected:
  EntityInterfacer *entityInterfacer; /*!< Pointer to EntityInterfacer object. */
  const bool logFlag;                 /*!< Log flag. */
  bool running;                       /*!< Running flag. */
  Data::Task executingTsk;            /*!< Current executing task. */
  std::string expectedDataType;

  /*! \brief Extracts a message.
   *
   * \param msg Message to be extracted of type string.
   * \param prefix Pointer to message prefix of type string.
   * \param body Pointer to message body of type string.
   * \return true if extraction is success. false otherwise.
   */
  bool extractMsg (const std::string& msg, std::string *prefix, std::string *body);

  /*! \brief Processes and acts upon a message.
   *
   * \param msg Message to be extracted of type string.
   * \param prefix Reference to message prefix of type string.
   * \param body Reference to message body of type string.
   */
  void processMsg (std::string msg, const std::string& prefix, const std::string& body);

  /*! \brief Processes and acts upon a message.
   *
   * \param msg Message to be extracted of type string.
   * \param prefix Reference to message prefix of type string.
   * \param body Reference to message body of type string.
   */
  void processData (char *msg, const unsigned int size);

  /*! \brief Sends a message to the entity manager.
   *
   * \param prefix Reference to the message prefix of type string.
   * \param body Reference to message body of type string.
   */
  void sendMsg (const std::string& prefix, const std::string& body = "");

  /*! \brief Sends a message to the entity manager.
   *
   * \param msg Message to be sent of type char pointer.
   * \param size Message size of type unsigned int.
   */
  void sendMsg (const char *msg, const unsigned int size);

};
typedef IceUtil::Handle <Communicator> CommunicatorPtr;

#endif /* COMMUNICATOR_H_ */
