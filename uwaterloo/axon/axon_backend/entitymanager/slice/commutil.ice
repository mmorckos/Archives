/*
 * commuti.ice
 *
 * ICE IDL definition for communications utilities.
 * 
 * Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef COMMUTIL_ICE_
#define COMMUTIL_ICE_

#include <Ice/BuiltinSequences.ice>
#include <Glacier2/Session.ice>

module CommUtil
{
  sequence <string> DataSeq;
  sequence <DataSeq> DataRecord;
};

#endif // COMMUTIL_ICE_
