/*
 * configparser.h
 *
 *  Created on: 2011-11-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include "parser.h"

#include <map>

namespace Util
{

  class ConfigParser : public Parser
  {

  public:
    /*! \brief Parses the given file and extracts a number of tokens and matching values.
     *
     * \param parsedData of type map to store the parsed data in.
     * \return true if file has a valid format. false otherwise.
     */
    bool parse (std::map <std::string, std::string> *parsedData);
  };
}

#endif /* CONFIGPARSER_H_ */
