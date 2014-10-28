/*
 * logger.h
 *
 *  Created on: 2011-11-07
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <sstream>

static const int BUFFSIZE_0 = 32;
static const int BUFFSIZE_1 = 8;

namespace Util
{
  class Logger
  {

  public:
    /*! \brief Constructor
     */
    Logger ();

    /*! \brief Destructor
     */
    virtual ~Logger ();

    /*! \brief Init function.
     *
     * Initializes the logging facility by opening a log file.
     *
     * \param fileName of type string which is the file name.
     * \param flag of type bool which signals whether to overwrite data in or append to data
     * in an existing file. Default is True for appending to file.
     * \return true if opening the file was successful. false otherwise.
     */
    bool init (const std::string fn, const bool flag = false);

    /*! \brief Logging function.
     *
     * Takes a string ID of logging entity and log message. Writes a time stamped
     * log message to log file.
     *
     * \param type of type string which is the type of the message to be logged.
     * \param id of type string which is the ID of the entity requesting to log.
     * \param msg of type string which is the message to be logged.
     * \param flag of type bool which signals whether to write the message to the log file or not.
     * \return true if logging with a timestamp was successful. false otherwise.
     */
    bool logMsg (const std::string type, const std::string id, const std::string msg);

    /*! \brief Close function
     *
     * Closes log file and does clean-up.
     */
    void close ();

  protected:
    std::string fileName; /*!< Log file name */
    std::ofstream file;   /*!< File descriptor */
    bool fileFlag;        /*!< Signals to write logs into a file */

    /*! \brief Timestamp generator.
     *
     * Generates a detailed timestamp to be used in logging.
     *
     * \param result of type string string which stores the timestamp.
     * \return true if generation was successful. false otherwise.
     */
    bool generateTimestamp (std::string *result);
  };
}

#endif /* LOGGER_H_ */
