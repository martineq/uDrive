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

InfoNode::InfoNode()  : Node("info") {
	// TODO Auto-generated constructor stub

}

InfoNode::~InfoNode() {
	// TODO Auto-generated destructor stub
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

void InfoNode::execute(MgConnectionW& conn, const char* url){

	vector<string> lista=split(conn->uri,'/');
	string userId=lista[3];
	string dirId=lista[5];

	if (!lista[4].compare("dir")){
		//Valido si el usuario existe en la base de datos
		// Obtener token del Header y chequear que exista para el userId solicitado.
		if (!1){
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"message\": \"user not found\", \"code\": %d, \"error_user_msg\": \"Usuario desconocido.\" }", MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			return;
		}
		//Si existe el dirId
		if (1){
			conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%i\",  \"name\": \"%s\",  \"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%i\", \"shared\": \"%s\",  \"lastModDate\": \"%s\"}, { \"id\": \"%i\",  \"name\": \"%s\", \"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%i\", \"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 1, "Carpeta-1", 500, "d", 1, "false", "10/10/2015", 2, "Archivo-1", 300, "a", 1, "false", "03/10/2015");
			return;
		}else{
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"message\": \"Bad Request\",  \"error_user_msg\": \"No existe directorio\"}");
		}

	}else{
		conn.sendStatus(MgConnectionW::STATUS_CODE_METHOD_NOT_ALLOWED);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("{ \"message\": \"Method not allowed\",  \"error_user_msg\": \"Metodo no permitido\"}");
	}
}

