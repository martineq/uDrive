#include "mg_connection.h"
#include "mongoose/mongoose.h"

extern "C" {
	#include <stdarg.h>
}

#include <cstdlib>
#include <cstring>

using std::atoi;
using std::string;
using std::strlen;

static const char* CONTENT_TYPES[] = {
	"application/json", // CONTENT_TYPE_JSON
	"text/html" // CONTENT_TYPE_HTML
};

MgConnection::MgConnection(struct mg_connection *c) : conn(c) {

}

void MgConnection::sendStatus(MgConnection::StatusCodes code){
	this->sendStatus( (int) code);
}

void MgConnection::sendStatus(int code){
	mg_send_status(this->conn, code);
}

void MgConnection::sendHeader(const string& name, const string& val){
	this->sendHeader(name.c_str(), val.c_str());
}

void MgConnection::sendHeader(const char* name, const char* val){
	mg_send_header(this->conn, name, val);
}

size_t MgConnection::printfData(const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	size_t ret = mg_vprintf_data(this->conn, fmt, ap);
	va_end(ap);
	return ret;
}

struct mg_connection* MgConnection::operator->(){
	return this->conn;
}

void MgConnection::sendContentType(const std::string& type){
	this->sendContentType(type.c_str());
}

void MgConnection::sendContentType(const char* type){
	this->sendHeader("Content-Type", type);
}

void MgConnection::sendContentType(MgConnection::ContentTypes type){
	if(type >= MgConnection::CONTENT_TYPE_TOTAL)
		return;
	this->sendContentType(CONTENT_TYPES[type]);
}

const std::string& MgConnection::getParameter(const string& key){
	return this->parameters[key];
}

void MgConnection::setParameter(const std::string& key, const string& value){
	this->parameters[key] = value;
}

string MgConnection::getVarStr(const char* varName, size_t max){
	string value;
	value.resize(max);
	// XXX: fixme es feo esto!,, jaja
	switch(mg_get_var(this->conn, varName, (char*) value.data(), max)){
		case -2:
			return this->getVarStr(varName, max+max);
			break;

		case -1:
			return string();
			break;

		default:
			break;
	}

	value.resize(strlen(value.data()));

	return value;
}

string MgConnection::getVarStr(const string& varName, size_t max){
	return this->getVarStr(varName.c_str(), max);
}


int MgConnection::getVarInt(const char* varName, size_t max){
	return atoi(this->getVarStr(varName, max).c_str());

}

int MgConnection::getVarInt(const string& varName, size_t max){
	return this->getVarInt(varName, max);
}


