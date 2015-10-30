#include "node.h"
#include <string>
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
	conn.sendStatus(MgConnectionW::STATUS_CODE_METHOD_NOT_ALLOWED);
	conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
	conn.printfData("{ \"message\":\"Method: '%s' in '%s' not allowed\"}", conn->request_method, conn->uri);
}
