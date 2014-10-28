/*
 * datarecord.h
 *
 *  Created on: 2012-02-29
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef DATARECORD_H_
#define DATARECORD_H_

#include <vector>
#include <iostream>

namespace Util
{
  class DataRecord
  {

  public:
    std::vector <std::vector <std::string> > dataList;

    DataRecord ();
    virtual ~DataRecord ();
  };
}

#endif /* SVCDATARECORD_H_ */
