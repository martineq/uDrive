#include "node.h"
#include <string>
#include <cstring>

using std::string;
using std::strncmp;

Node::Node(const char* str) : uri(str) {
}

bool Node::handle(MgConnection& conn, const char* url){
	if(this->match(url)){
		this->execute(conn, url);
		return true;
	}
	return false;
}

bool Node::match(const char* url){
	return strncmp(url, this->uri.c_str(), this->uri.length()) == 0;
}

void Node::execute(MgConnection& conn, const char* url){
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

void Node::executePost(MgConnection& conn, const char* url){
	this->methodNotAllowed(conn, url);
}

void Node::executeGet(MgConnection& conn, const char* url){
	this->methodNotAllowed(conn, url);
}

void Node::executeDelete(MgConnection& conn, const char* url){
	this->methodNotAllowed(conn, url);
}

void Node::methodNotAllowed(MgConnection& conn, const char* url){
	conn.sendStatus(MgConnection::STATUS_CODE_METHOD_NOT_ALLOWED);
	conn.sendContentType(MgConnection::CONTENT_TYPE_JSON);
	conn.printfData("{ \"message\": \" Method: '%s' en '%s' no manejado\", \"code\": 404, \"error_user_msg\": \"Ups... No se encontro!\" }", conn->request_method, conn->uri);
}
