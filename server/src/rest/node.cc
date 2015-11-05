#include "node.h"
#include "../db_constants.h"

#include <cstring>
#include "../util/log.h"

using std::string;
using std::strncmp;

Node::Node(const char* str) : uri(str) {
}

void Node::execute(MgConnectionW& conn, const char* url){
	const char *method = conn->request_method;
	if(strcmp(method, "GET") == 0)
		this->executeGet(conn, url);
	else if(strcmp(method, "POST") == 0)
		this->executePost(conn, url);
	else if(strcmp(method, "DELETE") == 0)
		this->executeDelete(conn, url);
	else
		this->methodNotAllowed(conn, url);
}

void Node::executePost(MgConnectionW& conn, const char* url){
	this->methodNotAllowed(conn, url);
}

void Node::executeGet(MgConnectionW& conn, const char* url){
	this->methodNotAllowed(conn, url);
}

void Node::executeDelete(MgConnectionW& conn, const char* url){
	this->methodNotAllowed(conn, url);
}

void Node::methodNotAllowed(MgConnectionW& conn, const char* url){
	Log(Log::LogMsgDebug) << "Method not allowed, method: " << conn->request_method << ", uri: "<< conn->uri;
	conn.sendStatus(MgConnectionW::STATUS_CODE_METHOD_NOT_ALLOWED);
	conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
	conn.printfData("{ \"message\":\"Method: '%s' in '%s' not allowed\"}", conn->request_method, conn->uri);
}

/**
 This method handler errors.

# define STATUS_OK 1
#define STATUS_USER_ALREADY_EXISTS 2
#define STATUS_DATABASE_ERROR 3
#define STATUS_KEY_NOT_FOUND 4

#define STATUS_WRONG_PASSWORD 5
#define STATUS_WRONG_TOKEN 6
#define STATUS_FAIL_SAVING_FILE 7
#define STATUS_FAIL_LOADING_FILE 8
#define STATUS_USER_FORBIDDEN 9
#define STATUS_MAX_QUOTA_EXCEEDED 10
#define STATUS_INVALID_URL 11
*/
std::string Node::handlerError(int status){
 switch(status)
   {
   case STATUS_USER_ALREADY_EXISTS :
       Log(Log::LogMsgDebug) << "ERROR: [" << "USER_ALREADY_EXISTS" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_DATABASE_ERROR :
       Log(Log::LogMsgDebug) << "ERROR: [" << "DATABASE_ERROR" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_KEY_NOT_FOUND :
       Log(Log::LogMsgDebug) << "ERROR: [" << "KEY_NOT_FOUND" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_WRONG_PASSWORD :
       Log(Log::LogMsgDebug) << "ERROR: [" << "WRONG_PASSWORD" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_WRONG_TOKEN :
       Log(Log::LogMsgDebug) << "ERROR: [" << "WRONG_TOKEN" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_FAIL_SAVING_FILE :
       Log(Log::LogMsgDebug) << "ERROR: [" << "FAIL_SAVING_FILE" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_FAIL_LOADING_FILE :
       Log(Log::LogMsgDebug) << "ERROR: [" << "FAIL_LOADING_FILE" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_USER_FORBIDDEN :
       Log(Log::LogMsgDebug) << "ERROR: [" << "USER_FORBIDDEN" << "] - uri: " << this->uri;
       return defaultResponse();

   case STATUS_MAX_QUOTA_EXCEEDED :
       Log(Log::LogMsgDebug) << "ERROR: [" << "MAX_QUOTA_EXCEEDED" << "] - uri: " << this->uri;
       return defaultResponse();

   case 11 :
       Log(Log::LogMsgDebug) << "ERROR: [" << "INVALID_URL" << "] - uri: " << this->uri;
       return defaultResponse();
   case 12 :
           Log(Log::LogMsgDebug) << "ERROR: [" << "EMPTY DIR" << "] - uri: " << this->uri;
           return defaultResponse();
   default :
       Log(Log::LogMsgDebug) << "[" << "NOT ERROR DEFINED" << "] - uri: " << this->uri;
       return defaultResponse();
   }
}

std::string Node::defaultResponse(){};