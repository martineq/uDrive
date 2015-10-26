#include "mg_connection_w.h"
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

MgConnectionW::MgConnectionW(struct mg_connection *c) : conn(c) {
	multipartOffset=0;

}

std::string MgConnectionW::getMultipartData(string& var_name, string& file_name){
	const char *data = NULL;
	int data_len = 0;

	var_name.resize(100);
	file_name.resize(100);

	var_name[0] = 0;
	file_name[0] = 0;

	this->multipartOffset = mg_parse_multipart(this->conn->content + this->multipartOffset,this->conn->content_len - this->multipartOffset,
		(char*) var_name.data(), 100,
		(char*) file_name.data(), 100,
		&data, &data_len
	);

	var_name.resize(strlen(var_name.data()));
	file_name.resize(strlen(file_name.data()));

	return string(data, data_len);
}

void MgConnectionW::sendStatus(MgConnectionW::StatusCodes code){
	this->sendStatus( (int) code);
}

void MgConnectionW::sendStatus(int code){
	mg_send_status(this->conn, code);
}

void MgConnectionW::sendHeader(const string& name, const string& val){
	this->sendHeader(name.c_str(), val.c_str());
}

string MgConnectionW::getAuthorization(){
	const char* dar=mg_get_header(this->conn,"Authorization");
	std::string my_string(dar);
	return my_string;
}

void MgConnectionW::sendHeader(const char* name, const char* val){
	mg_send_header(this->conn, name, val);
}

size_t MgConnectionW::printfData(const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	size_t ret = mg_vprintf_data(this->conn, fmt, ap);
	va_end(ap);
	return ret;
}

struct mg_connection* MgConnectionW::operator->(){
	return this->conn;
}

void MgConnectionW::sendContentType(const std::string& type){
	this->sendContentType(type.c_str());
}

void MgConnectionW::sendContentType(const char* type){
	this->sendHeader("Content-Type", type);
}

void MgConnectionW::sendContentType(MgConnectionW::ContentTypes type){
	if(type >= MgConnectionW::CONTENT_TYPE_TOTAL)
		return;
	this->sendContentType(CONTENT_TYPES[type]);
}

const std::string& MgConnectionW::getParameter(const string& key){
	return this->parameters[key];
}

void MgConnectionW::setParameter(const std::string& key, const string& value){
	this->parameters[key] = value;
}

string MgConnectionW::getVarStr(const char* varName, size_t max){
	string value;
	value.resize(max);
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

string MgConnectionW::getVarStr(const string& varName, size_t max){
	return this->getVarStr(varName.c_str(), max);
}

int MgConnectionW::getVarInt(const char* varName, size_t max){
	return atoi(this->getVarStr(varName, max).c_str());
}

int MgConnectionW::getVarInt(const string& varName, size_t max){
	return this->getVarInt(varName, max);
}


