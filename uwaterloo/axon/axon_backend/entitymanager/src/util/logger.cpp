/*
 * logger.cpp
 *
 *  Created on: 2011-11-07
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#if defined (__linux__)
#include <sys/time.h>
#include <string.h>
#endif
#if defined (_WIN32)
#include <sys/timeb.h>
#include <time.h>
#endif
#include <assert.h>

#include "logger.h"

using namespace Util;

/*! \brief Constructor
 */
Logger::Logger ():
  file (0)
{}

/*! \brief Destructor
 */
Logger::~Logger ()
{
  close ();
}

bool
Logger::init (const std::string fileName, bool flag)
{
  this->fileName = fileName;
  if (file.is_open ())
      file.close ();

  try
    {
      file.open (fileName.c_str (), flag ? std::ios::app : std::ios::out);
    }
  catch (std::ifstream::failure ex)
    {
      std::cout << "ERROR! Logger: Failed to open file." << std::endl;
      return false;
    }

  return file.is_open ();
}

bool
Logger::logMsg (const std::string type, const std::string id, const std::string msg,
    const bool flag)
{
  assert (file.is_open ());

  std::string timestamp;
  bool result;

  if (file.good ())
    {
      result = generateTimestamp (&timestamp);

      std::cout << "** " << timestamp << " -- " << id <<  ": " << type << ": " << msg << std::endl;
      if (flag)
          file << "** " << timestamp << " -- " << id <<  ": " << type << ": " << msg << std::endl;

      if (result)
          return true;
      else
          return false;
    }

  return false;
}

void
Logger::close ()
{
  file.close ();
}

bool
Logger::generateTimestamp (std::string *result)
{
  time_t currTime;
  char buff0 [BUFFSIZE_0];
  char buff1 [BUFFSIZE_1];
#if defined (__linux__)
  struct tm *timeS;
  struct timeval milli;
#elif defined (_WIN32)
  struct tm timeS;
  struct timeb milli;
  int err;
#endif

  currTime = time (NULL);
#if defined (__linux__)
  timeS = localtime (&currTime);
  gettimeofday (&milli, NULL);
  strncpy (buff0, asctime (timeS), BUFFSIZE_0);
  sprintf (buff1, ".%ld", milli.tv_usec / 1000);
#elif defined (_WIN32)
  err = localtime_s (&timeS, &currTime);

  if (err)
    {
      strerror_s (buff0, BUFFSIZE_0, errno);
      result->append (buff0);
      return false;
    }

  ftime (&milli);
  asctime_s (buff0, BUFFSIZE_0, &timeS);
  sprintf_s (buff1, ".%ld", milli.millitm);
#endif

  result->append ("[");
  result->append (buff0);
  result->erase (result->size() - 1);
  result->insert (result->size() - 5, buff1, strlen (buff1));
  result->append ("]");

  return true;
}
