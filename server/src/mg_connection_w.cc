#include "mg_connection_w.h"

extern "C" {
	#include <stdarg.h>
}

#include <cstring>
#include <json/json.h>
#include <map>

using std::vector;

static const char* CONTENT_TYPES[] = {
	"application/json", // CONTENT_TYPE_JSON
	"text/html" // CONTENT_TYPE_HTML
    "multipart/form-data" // CONTENT_TYPE_MULTIPART
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

size_t MgConnectionW::setMultipartData(string var_name, string file_name,const void *data, int data_len){
    var_name.resize(100);
    file_name.resize(100);
    var_name[0] = 0;
    file_name[0] = 0;
    Log(Log::LogMsgDebug) << "[OFFSET]: "<<this->multipartOffset << "[DATA_LEN]: "<<data_len;
    this->multipartOffset = mg_send_data(this->conn, data, data_len);
    var_name.resize(strlen(var_name.data()));
    file_name.resize(strlen(file_name.data()));

    return this->multipartOffset;
}

string MgConnectionW::getAuthorization(){
    Log(Log::LogMsgDebug) << "[getAuthorization]: Recuperando token Header";
	const char* dar=mg_get_header(this->conn,"Authorization");
    Log(Log::LogMsgDebug) << "[getAuthorization]: Token: "<<dar;
    if (dar==NULL) return "";
	std::string my_string(dar);
	return my_string;
}
/**
 *
 */
std::string MgConnectionW::getBodyJson(string field) {
    Log(Log::LogMsgDebug) << "[getBodyJson]: Recuperando campo Json";
    std::string content_string(this->conn->content);
    Log(Log::LogMsgDebug) << "[getBodyJson]: Tamaño body: "<<getContentLength();
    Log(Log::LogMsgDebug) << "[getBodyJson]: Campo: "<<field;

    Json::Value root;
    Json::Reader reader;
    bool parsedSuccess = reader.parse(content_string, root, false);
    if (!parsedSuccess) {
        Log(Log::LogMsgDebug) << "[getBodyJson]: Error parseando Body";
        return "";
    }
    if (root.size()!=0) {
        Log(Log::LogMsgDebug) << "[getBodyJson], recuperando valor correctamente";
        return root[field].asString();
    }
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
    const char* metodo=method.c_str();
	conn->request_method=metodo;
};

std::string MgConnectionW::getContentLength(){
    return std::to_string((size_t)conn->content_len);

}

vector<string> split(const string &s, char delim) {
    std::stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

std::string MgConnectionW::getParameter(std::string key){
    Log(Log::LogMsgDebug) << "[getParameter]: Recuperando parameters, key: "<<key;
    const char* query=conn->query_string;

    if (query==NULL){
        Log(Log::LogMsgDebug) << "[getParameter]: query is NULL";
        return "";
    }
    std::string my_string(query);
    Log(Log::LogMsgDebug) << "[getParameter]: Querry: "<< my_string;

    vector<string> lista=split(query,'=');
    string k=lista[0];
    Log(Log::LogMsgDebug) << "[getParameter]: read key: "  <<k;

    if (k==key) {
        Log(Log::LogMsgDebug) << "[getParameter]: retrive content of key.";
        return lista[1];
    }
    else {
        Log(Log::LogMsgDebug) << "[getParameter]: key not present.";
        return "";
    }
}