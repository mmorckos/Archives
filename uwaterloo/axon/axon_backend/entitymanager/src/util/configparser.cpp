/*
 * configparser.cpp
 *
 *  Created on: 2011-11-15
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include <assert.h>

#include "configparser.h"

using namespace Util;

bool
ConfigParser::parse (std::map <std::string, std::string> *parsedData)
{
  assert (file.is_open ());

  std::string line;
  std::string token;
  std::string value;
  size_t index;

  while (!file.eof ())
    {
      getline (file, line);
      ++lineNum;

      if (line [0] == commentChar || line [0] == '\0')
          continue;

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

      parsedData->insert (std::pair <std::string, std::string> (token, value));
    }

  setSuccMsg ();
  return true;
}
