#include "node.h"

#include <cstring>

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
/#define STATUS_OK 1
#define STATUS_USER_ALREADY_EXISTS 2
#define STATUS_DATABASE_ERROR 3
#define STATUS_KEY_NOT_FOUND 4
#define STATUS_WRONG_PASSWORD 5
#define STATUS_WRONG_TOKEN 6
#define STATUS_FAIL_SAVING_FILE 7
#define STATUS_FAIL_LOADING_FILE 8
#define STATUS_USER_FORBIDDEN 9
#define STATUS_MAX_QUOTA_EXCEEDED 10
*/
std::string Node::handlerError(int status){};
/*
 switch(status)
   {
   case 'STATUS_USER_ALREADY_EXISTS' :
       
      break;
   case 'B' :
   case 'C' :
      cout << "Well done" << endl;
      break;
   case 'D' :
      cout << "You passed" << endl;
      break;
   case 'F' :
      cout << "Better try again" << endl;
      break;
   default :
      cout << "Invalid grade" << endl;
   }

};*/