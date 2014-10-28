/*
 * xmlprofileparser.cpp
 *
 *  Created on: 2012-04-03
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#include "xmlprofileparser.h"

XmlProfileParser::XmlProfileParser ()
{}

XmlProfileParser::~XmlProfileParser ()
{}

bool
XmlProfileParser::parse (Data::BasicProfile *basicProfile)
{
  pugi::xml_node basicProfileXmlnode;
  std::string value;

  if (!inFile.load_file (fileName.c_str ()))
    {
      internalStatus = "Unable to parse XML file.";
      return false;
    }

  basicProfileXmlnode = inFile.child ("BasicProfile");

  value = "";
  value = basicProfileXmlnode.attribute ("name").value ();
  if (value.size () > 0)
    {
      basicProfile->set_name (value);
    }
  else
    {
      internalStatus = "Missing <name> attribute.";
      return false;
    }

  value = "";
  value = basicProfileXmlnode.attribute ("category").value ();
  if (value.size () > 0)
    {
      if (value == "ROBOT")
        {
          basicProfile->set_category (Data::BasicProfile_Category_ROBOT);

          value = "";
          value = basicProfileXmlnode.attribute ("type").value ();
          if (value.size () > 0)
            {
              if (value == "MOBILE")
                {
                  basicProfile->set_type (Data::BasicProfile_Type_MOBILE);
                }
              /// If robotic entity is stationary it must have a fixed position in the environment
              else if (value == "STATIONARY")
                {
                  basicProfile->set_type (Data::BasicProfile_Type_STATIONARY);

                  value = "";
                  value = basicProfileXmlnode.attribute ("xpos").value ();
                  if (value.size () > 0)
                    {
                      basicProfile->set_xpos (basicProfileXmlnode.attribute ("xpos").as_double ());
                    }
                  else
                    {
                      internalStatus = "Missing value for <xpos> attribute.";
                      return false;
                    }

                  value = "";
                  value = basicProfileXmlnode.attribute ("ypos").value ();
                  if (value.size () > 0)
                    {
                      basicProfile->set_ypos (basicProfileXmlnode.attribute ("ypos").as_double ());
                    }
                  else
                    {
                      internalStatus = "Missing value for <ypos> attribute.";
                      return false;
                    }

                  value = "";
                  value = basicProfileXmlnode.attribute ("zpos").value ();
                  if (value.size () > 0)
                    {
                      basicProfile->set_zpos (basicProfileXmlnode.attribute ("zpos").as_double ());
                    }
                  else
                    {
                      internalStatus = "Missing value for <zpos> attribute.";
                      return false;
                    }
                }
              else
                {
                  internalStatus = "Unknown value for <type> attribute.";
                  return false;
                }
            }
          else
            {
              internalStatus = "Missing <type> attribute.";
              return false;
            }
        }
      else if (value == "MACHINE")
        {
          basicProfile->set_category (Data::BasicProfile_Category_MACHINE);
        }
      else
        {
          internalStatus = "Unknown value for <category> attribute.";
          return false;
        }
    }
  else
    {
      internalStatus = "Missing <name> attribute.";
      return false;
    }

  basicProfile->set_descr (basicProfileXmlnode.attribute ("descr").value ());

  return true;
}
