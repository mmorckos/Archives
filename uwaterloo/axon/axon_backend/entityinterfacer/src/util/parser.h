/*
 * parser.h
 *
 *  Created on: 2011-11-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <fstream>
#include <sstream>

static const char CMTCHAR = '#';

class Parser
{

public:
  /*! \brief Constructor
   */
  Parser ():
    commentChar (CMTCHAR),
    lineNum (0)
  {}

  /*! \brief Destructor
   */
  virtual ~Parser ()
  {
    file.close ();
  }

  /*! \brief Function to setup and open the configurations file to read.
   *
   * \param fileName configurations file name.
   * \return true if opening the file was successful. false otherwise.
   */
  bool setFile (std::string fileName)
  {
    this->fileName = fileName;

    if (file.is_open ())
        file.close ();

    try
    {
      file.open (fileName.c_str ());
    }
    catch (std::ifstream::failure& ex)
    {
      internalStatus = "Failed to open file due to an exception.";
      return false;
    }

    if (!file.is_open ())
        internalStatus = "Failed to open file.";

    return file.is_open ();
  }

  /*! \brief Sets the commenting key character to be used in parsing.
   *
   * \param ch the commenting character in the configurations file (Default is '#').
   */
  void setCommentChar (char ch)
  {
    commentChar = ch;
  }

  std::string getInternalStatus ()
  {
    return internalStatus;
  }

protected:
  std::string fileName; /*!< Configurations file name. */
  std::ifstream file;   /*!< File descriptor. */
  char commentChar;
  int lineNum;
  std::string internalStatus;

  void setErrMsg ()
  {
    std::ostringstream oss;

    oss << "Parsing error at line: " << lineNum << ".";
    internalStatus = oss.str ();
  }

  void setSuccMsg ()
  {
    internalStatus = "Parsing done. Success.";
  }
};

#endif /* PARSER_H_ */
