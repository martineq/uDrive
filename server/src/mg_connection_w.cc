#include "mg_connection_w.h"
#include "mongoose/mongoose.h"

extern "C" {
	#include <stdarg.h>
}

#include <cstring>
#include <json/json.h>

using std::atoi;
using std::string;
using std::strlen;

static const char* CONTENT_TYPES[] = {
	"application/json", // CONTENT_TYPE_JSON
	"text/html" // CONTENT_TYPE_HTML
};

MgConnectionW::MgConnectionW(struct mg_connection *c) : conn(c),multipartOffset(0) {
}

void MgConnectionW::sendStatus(int code){
	mg_send_status(this->conn, code);
}

std::string MgConnectionW::getMultipartData(string& var_name, string& file_name){
	const char *data = NULL;
	int data_len = 0;
	var_name.resize(100);
	file_name.resize(100);
	var_name[0] = 0;
	file_name[0] = 0;
    Log(Log::LogMsgDebug) << "[OFFSET]: "<<this->multipartOffset << "[DATA_LEN]: "<<data_len;
	this->multipartOffset = mg_parse_multipart(this->conn->content + this->multipartOffset,this->conn->content_len - this->multipartOffset,
		(char*) var_name.data(), 100,
		(char*) file_name.data(), 100,
		&data, &data_len
	);
	var_name.resize(strlen(var_name.data()));
	file_name.resize(strlen(file_name.data()));

	return string(data, data_len);
}

string MgConnectionW::getAuthorization(){
	const char* dar=mg_get_header(this->conn,"Authorization");
	std::string my_string(dar);
	return my_string;
}

std::string MgConnectionW::getBodyJson(string field) {
    char body[1024 * 1024 * sizeof(char)] = "";

    strncpy(body, this->conn->content, this->conn->content_len);
    Log(Log::LogMsgDebug) << "[getBodyJson]";
    body[this->conn->content_len] = '0';

// Parse the JSON body
    Json::Value root;
    Json::Reader reader;
    bool parsedSuccess = reader.parse(body, root, false);
    if (!parsedSuccess) {
        Log(Log::LogMsgDebug) << "[getBodyJson]: Error parseando Body";
        return "";
    }
    if (root != "") return root[field].asString();
    else {
        Log(Log::LogMsgDebug) << "[getBodyJson]: No se encontro el campo: "<<field;
        return "";
    }
}

struct mg_connection* MgConnectionW::operator->(){
	return this->conn;
}

void MgConnectionW::sendContentType(MgConnectionW::ContentTypes type){
    mg_send_header(this->conn, "Content-Type", CONTENT_TYPES[type]);
}

size_t MgConnectionW::printfData(const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    size_t ret = mg_vprintf_data(this->conn, fmt, ap);
    va_end(ap);
    return ret;
}

const char* MgConnectionW::getUri(){
	return conn->uri;
};

void MgConnectionW::setUri(string uri_aux){
	conn->uri=uri_aux.c_str();
}

const char* MgConnectionW::getMethod(){
	return conn->request_method;
};

void MgConnectionW::setMethod(std::string method){
	conn->request_method=method.c_str();
};

std::string MgConnectionW::getContentLength(){
    return std::to_string((size_t)conn->content_len);

}



