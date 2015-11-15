#include "node.h"
#include <cstring>

using std::string;

Node::Node(MgConnectionW& conn){
    setConnection(conn);
}

void Node::execute() {
	const char *method = getConnection().getMethod();
    int status=11;
    if (this->auth(status)) {
        if (strcmp(method, "GET") == 0) this->executeGet();
        else if (strcmp(method, "POST") == 0)  this->executePost();
        else if (strcmp(method, "DELETE") == 0) this->executeDelete();
        else if (strcmp(method, "PUT") == 0) this->executePut();
        else this->methodNotAllowed();
    }else this->requestForbidden(status);
}

std::string Node::getUserId() {return "0";}

bool Node::auth(int &status) {
    if (getConnection().getAuthorization()=="") {
        Log(Log::LogMsgDebug) << "[" << "Auth node" << "] UserId: " <<getUserId()<<" Not Authorization information";
        return false;
    }
    Log(Log::LogMsgDebug) << "[" << "Auth node" << "] UserId: " <<getUserId() <<" Token: "<<getConnection().getAuthorization();
    return getRequestDispatcher()->check_token(getUserId(),getConnection().getAuthorization(),status);
}

void Node::executePost() {
    this->methodNotAllowed();
}

void Node::executeGet() {
    this->methodNotAllowed();
}

void Node::executeDelete() {
    this->methodNotAllowed();
}

void Node::executePut() {
    this->methodNotAllowed();
}

void Node::methodNotAllowed() {
	Log(Log::LogMsgDebug) << "Method not allowed, method: " << getConnection().getMethod()<< ", connection: "<< getConnection().getUri();
    getConnection().sendStatus(MgConnectionW::STATUS_CODE_METHOD_NOT_ALLOWED);
    getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
    getConnection().printfData("{ \"message\":\"Method: '%s' in '%s' not allowed\"}", getConnection().getMethod(), getConnection().getUri());
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
       Log(Log::LogMsgDebug) << "ERROR: [" << "USER_ALREADY_EXISTS" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_DATABASE_ERROR :
       Log(Log::LogMsgDebug) << "ERROR: [" << "DATABASE_ERROR" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_KEY_NOT_FOUND :
       Log(Log::LogMsgDebug) << "ERROR: [" << "KEY_NOT_FOUND" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_WRONG_PASSWORD :
       Log(Log::LogMsgDebug) << "ERROR: [" << "WRONG_PASSWORD" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_WRONG_TOKEN :
       Log(Log::LogMsgDebug) << "ERROR: [" << "WRONG_TOKEN" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_FAIL_SAVING_FILE :
       Log(Log::LogMsgDebug) << "ERROR: [" << "FAIL_SAVING_FILE" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_FAIL_LOADING_FILE :
       Log(Log::LogMsgDebug) << "ERROR: [" << "FAIL_LOADING_FILE" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_USER_FORBIDDEN :
       Log(Log::LogMsgDebug) << "ERROR: [" << "USER_FORBIDDEN" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case STATUS_MAX_QUOTA_EXCEEDED :
       Log(Log::LogMsgDebug) << "ERROR: [" << "MAX_QUOTA_EXCEEDED" << "] - connection: " << getConnection().getUri();
       return defaultResponse();

   case 11 :
       Log(Log::LogMsgDebug) << "ERROR: [" << "INVALID_URL" << "] - connection: " << getConnection().getUri();
       return defaultResponse();
   case 12 :
           Log(Log::LogMsgDebug) << "ERROR: [" << "EMPTY DIR" << "] - connection: " << getConnection().getUri();
           return defaultResponse();
   default :
       Log(Log::LogMsgDebug) << "[" << "NOT ERROR DEFINED" << "] - connection: " << getConnection().getUri();
       return defaultResponse();
   }
}
std::string Node::defaultResponse(){};

void Node::setRequestDispatcher(RequestDispatcher* rd){
    this->rd=rd;
}

RequestDispatcher* Node::getRequestDispatcher(){
    return  this->rd;
}

const char* Node::getUri(){
    return getConnection().getUri();
};

MgConnectionW Node::getConnection() {
    return this->connection;
};

void Node::setConnection(MgConnectionW& conn1) {
    this->connection=conn1;
};

void Node::requestForbidden(int status) {
    Log(Log::LogMsgDebug) <<  "[ " <<"Request Forbidden - Node " << "]";
    getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
    getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
    string msg=handlerError(STATUS_WRONG_TOKEN);
    getConnection().printfData(msg.c_str());
}