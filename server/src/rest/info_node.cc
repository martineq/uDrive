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

	std::cout << lista.size() << std::endl;

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		//Validacion del token que se recibe.
		string userId=lista[3];
		string dirId=lista[5];
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << conn.getAuthorization() << " UserID: " << userId;

		int status;
		string token=conn.getAuthorization();
		//BD: Consulto en la BD el token de este userid

		if (token.compare("be16e465de64f0d2f2d83f3cfcd6370b")!=0){ //BD:Valido si el mail existe en la base de datos
			Log(Log::LogMsgDebug) << "[" << "invalid token" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"id\": \"%d\",  \"name\": \"%s\",  "
									"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
									"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}", 0, "", 0,"",0,"","");
		}else if (dirId.compare("1000")!=0){
			Log(Log::LogMsgDebug) << "[" << "invalid dirId" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"id\": \"%d\",  \"name\": \"%s\",  "
												"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
												"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}", 0, "", 0,"",0,"","");
		}else if (userId.compare("1111")!=0){
			Log(Log::LogMsgDebug) << "[" << "invalid userId" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"id\": \"%d\",  \"name\": \"%s\",  "
												"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
												"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}", 0, "", 0,"",0,"","");
		}else{
			Log(Log::LogMsgDebug) << "[" << "retrieve list" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"id\": \"%d\",  \"name\": \"%s\",  "
												"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
												"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}", 1000, "Carpeta1", 1024,"d",2,"true","10/08/2015");
		}
	}else{
		Log(Log::LogMsgDebug) << "[" << "invalid url" << "]";
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("{ \"id\": \"%d\",  \"name\": \"%s\",  "
											"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
											"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}", 0, "", 0,"",0,"","");

	}

}

