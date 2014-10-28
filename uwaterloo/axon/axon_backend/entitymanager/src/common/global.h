/*
 * global.h
 *
 *  Created on: 2011-12-08
 *      Author: Michael Morckos <mmorckos@uwaterloo.ca>
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

/*********************************/
/************ NAMES ************/

/** Environment **/
const std::string ENTITY_SERVER = "ENTSERV";
const std::string FRONTEND_SERVER = "FRNTNDSERV";

/** Entity **/
const std::string ENTITY = "ENT";
const std::string ENTITY_MANAGER = "EntityManager";
const std::string ENTITY_INTERFACER = "EntityInterfacer";
const std::string ENTITY_CLIENT = "ENTCLNT";

/** Client **/
const std::string CLIENT = "CLNT";
const unsigned int CLIENT_MSG = 0;
const unsigned int ENVIR_MSG = 1;
const unsigned int ENTITY_MSG = 2;

/*********************************/
/************ MESSAGES ************/

/** (Entity/Client) --> (Server) messages types **/
const std::string INIT = "INIT";
const std::string INQUIRY = "INQ";

/** (Entity/Client) --> (Server) messages **/
const std::string PING = "PING";

/** (Server) --> (Entity/Client) message types **/
const std::string INFO = "INFO";
const std::string DATA = "DATA";
const std::string REQUEST = "REQ";
const std::string ALERT = "ALERT";
const std::string WARN = "WARN";
const std::string ERROR = "ERR";
const std::string BAD_MSG = "BADMSG";
const std::string BAD_PARAM = "BADPARAM";
const std::string BAD_RES = "BADRES";
const std::string BAD_TSK = "BADTSK";
const std::string OK_MSG = "OKMSG";
const std::string OK_PARAM = "OKPARAM";
const std::string OK_RES = "OKRES";
const std::string OK_TSK = "OKTSK";
const std::string DENY_MSG = "DNYMSG";
const std::string DENY_PARAM = "DNYPARAM";
const std::string DENY_RES = "DNYRES";
const std::string DENY_TSK = "DNYTSK";
const std::string DONE_TSK = "DONETSK";
const std::string OK_REGISTERED = "OKREG";
const std::string OK_UNREGISTERED = "OKUNREG";
const std::string OK_CHECKOUT = "OKCHKOUT";
const std::string OK_RELEASE = "OKREL";
const std::string OK_STOPPED = "OKSTP";
const std::string UNKNOWN_DATA = "UNKWNDAT";

const std::string UNKNOWN_MSG = "UKNMSG";
const std::string PROBE = "PROBE";
const std::string NONE = "NONE";
const std::string CLIENT_INFOMSG = "CLNTINFOMSG";
const std::string TSK_FDBK = "TSKFDBK";

/** (Server) --> (Entity/Client) messages **/
const std::string REGISTERED = "REG";
const std::string UNREGISTERED = "UNREG";
const std::string ENTITY_REGISTERED = "ENTREG";
const std::string ENTITY_UNREGISTERED = "ENTUNREG";
const std::string CLIENT_REGISTERED = "CLNTREG";
const std::string CLIENT_UNREGISTERED = "CLNTUNREG";
const std::string CLIENT_NOT_EXIST = "CLNTNOTEXST";

const std::string CHECKOUT_ENTITY = "CHKOUTENT";
const std::string ENTITY_GRANTED = "ENTGRANT";
const std::string ENTITY_DENIED = "ENTDENY";
const std::string ENTITY_CONTROL_DENIED = "ENTCTRLDENY";
const std::string RELEASE_ENTITY = "ENTREL";
const std::string ENTITY_RELEASED = "ENTRELED";
const std::string ENTITY_ALREADY_RELEASED = "ALRDYENTRELED";

const std::string ENTITY_DROPPED = "ENTDRPED";
const std::string ENTITY_NOEXIST = "ENTNOEXIST";

const std::string DATAREQ = "DATAREQ";
const std::string NODATA = "NODATA";

const std::string COLLABORATION = "COLLAB";
const std::string COLLABORATION_OK = "COLLABOK";
const std::string TERMINATE_COLLABORATION = "TERMCOLLAB";

const std::string CLIENT_TASK = "CLNTTSK";
const std::string CLIENT_COLLABORATIVE_TASK = "CLNTCOLLABTSK";

const std::string RECEIVED = "RECEIVED";
const std::string PONG = "PONG";

const std::string SUCCESS = "SUCC";
const std::string FAILURE = "FAILURE";

/*********************************/
/************ ENTITY ************/

const std::string PRFL = "PRFL";
const std::string SVCPRFL = "SVCPRFL";
const std::string STAT = "STAT";
const std::string IDLE = "IDLE";
const std::string READY = "READY";
const std::string BUSY = "BUSY";
const std::string SVC = "SVC";
const std::string GLOBALSVC = "GBLSVC";
const std::string CTRLPARAM = "CTRLPARAM";
const std::string PARAM = "PARAM";
const std::string RESOURCE = "RES";
const std::string TSK = "TSK";
const std::string POS = "POS";
const std::string ACK = "ACK";
const std::string SVCUPDT = "SVCUPDT";
const std::string STOP = "STP";
const std::string EMRGSTP = "EMRGSTP";
const std::string HLTH = "HLTH";
const std::string FDBK = "FDBK";

/*********************************/
/************ GENERAL ************/

/** Messages delimiter **/
const size_t BUFFSIZE = 2048;
static const char *DELIM = "_";

#endif /* GLOBAL_H_ */
