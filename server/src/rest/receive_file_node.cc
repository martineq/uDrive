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

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		std::string userId=lista[3];
		std::string dirId=lista[5];
		std::string file_id;
		std::string token=conn.getAuthorization();
		char* p_file;

		Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] token: " << token << " UserID: " << userId;
		Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] userId: " << userId << " dirId: " << dirId;
		int status;
		std::string var_name;
		std::string file_name;
		std::string arch;

		time_t now = time(0);
		char* dt = ctime(&now);
		std::string fecha(dt);

		while((arch = conn.getMultipartData(var_name, file_name)) != ""){
			Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "], Var_name: " << var_name << ", file_name: " << file_name;
			if(var_name == "arch") break;
		}
		Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], finished" << ", status: " << status;


		if(!this->rd->new_file(userId, token, var_name, ".jpg",fecha, p_file,"1024",dirId,file_id,status)){
			Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] invalid token";
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
														"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
														"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
		}else{
			//Autorizado
			Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], authorized";
			
			

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
		Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode" << "] invalid url";
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

