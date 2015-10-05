#include "node.h"
#include <string>
#include <cstring>

using std::string;
using std::strncmp;

Node::Node(const char* str) : uri(str) {
}

bool Node::handle(MgConnectionW& conn, const char* url){
	if(this->match(url)){
		this->execute(conn, url);
		return true;
	}
	return false;
}

bool Node::match(const char* url){
	return strncmp(url, this->uri.c_str(), this->uri.length()) == 0;
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
	conn.printfData("{ \"message\": \" Method: '%s' en '%s' no manejado\", \"code\": 404, \"error_user_msg\": \"Ups... No se encontro!\" }", conn->request_method, conn->uri);
}
