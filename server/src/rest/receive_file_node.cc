/*
 * receive_file_node.h
 *
 *  Created on: 26/10/2015
 *      Author: martin
 */

#include "receive_file_node.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;


ReceiveFileNode::ReceiveFileNode() : Node("ReceiveFileNode") {
}

ReceiveFileNode::~ReceiveFileNode() {
}
vector<string> ReceiveFileNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ReceiveFileNode::executePost(MgConnectionW& conn, const char* url){
	vector<string> lista=ReceiveFileNode::split(conn->uri,'/');
	Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] "; 

	if ( (!lista[3].compare("dir")) && (lista.size()==5)){
		std::string userId=lista[2];
		std::string dirId=lista[4];
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << conn.getAuthorization() << " UserID: " << userId;

	//	int status;
		std::string token=conn.getAuthorization();
	//	std::string var_name;
	//	std::string file_name;
	//	std::string arch;

	//	time_t now = time(0);
	//	char* dt = ctime(&now);
	//	std::string fecha(dt);



		//if(this->rd->new_file(userId, token, var_name, ".jpg",fecha, const char* p_file_stream, string size, string parent_dir_id, string& file_id, int& status){



		if (token.compare("be16e465de64f0d2f2d83f3cfcd6370b")!=0){
			Log(Log::LogMsgDebug) << "[" << "invalid token" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
														"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
														"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
		}else{
			//Autorizado
			Log(Log::LogMsgInfo) << "[" << "Receive file" << "], authorized";
			
			std::string var_name;
			std::string file_name;
			std::string arch;

			while((arch = conn.getMultipartData(var_name, file_name)) != ""){
				Log(Log::LogMsgDebug) << "[" << "Receive file" << "], Var_name: " << var_name << ", file_name: " << file_name << " Contenido: " << arch;
				if(var_name == "arch") break;
			}
			Log(Log::LogMsgInfo) << "[" << "Receive file" << "], finished";

			//Tomo tiempo de ultima modificacion
			time_t now = time(0);
			char* dt = ctime(&now);
			conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%s\",  \"name\": \"%s\","
														"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
														"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", dirId.c_str(), "Carpeta1", 0,"d",1,"",dt);
		}
	}else{
		Log(Log::LogMsgDebug) << "[" << "invalid url" << "]";
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
											"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
											"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");

	}
}
void ReceiveFileNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}

