/*
 * info_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: root
 */

#include "info_node.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::vector;

InfoNode::InfoNode()  : Node("Info") {
}

InfoNode::~InfoNode() {
}

vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void InfoNode::executeGet(MgConnectionW& conn, const char* url){
	vector<string> lista=split(conn->uri,'/');
	if (!lista[4].compare("dir")){
		//Validacion del token que se recibe.
		string userId=lista[3];
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << conn.getAuthorization() << " UserID: " << userId;

		int status;
		string token;
		//BD: Consulto en la BD el token de este userid

		if (!1){ //Comparo el token recibido con el token de la BD
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"message\": \"user not found\", \"code\": %d, \"error_user_msg\": \"Usuario desconocido.\" }", MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			return;
		}
		Log(Log::LogMsgDebug) << "[" << "valid user" << "]";
		string dirId=lista[5];

		if (1){ //BD: Compruebo si existe el dirId
			conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"id\": \"%i\",  \"name\": \"%s\",  "
				"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%i\", "
				"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}", 1, "Carpeta", 500,"a",1,"false","10/10/2015");
			return;
		}else{
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"message\": \"Bad Request\",  \"error_user_msg\": \"No existe directorio\"}");
		}

	}else{ //
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("{ \"message\": \"Method not allowed\",  \"error_user_msg\": \"Metodo no permitido\"}");
	}
}

