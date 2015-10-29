/*
 * create_dir_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "create_dir_node.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::vector;

CreateDirNode::CreateDirNode()  : Node("CreateDir") {
}

CreateDirNode::~CreateDirNode() {
}

vector<string> CreateDirNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void CreateDirNode::executePost(MgConnectionW& conn, const char* url){
	vector<string> lista=CreateDirNode::split(conn->uri,'/');
	
	if ( (!lista[3].compare("dir")) && (lista.size()==5)){
		string userId=lista[2];
		string dirId=lista[4];
		string token=conn.getAuthorization();
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;

		const char *s = conn->content;
		char body[1024*sizeof(char)] = "";
		strncpy(body, s, conn->content_len);
		body[conn->content_len] = '0';

		// Parse the JSON body
		Json::Value root;
		Json::Reader reader;
		bool parsedSuccess = reader.parse(body, root, false);
		if (!parsedSuccess) {
			// Error, do something
		}
		const Json::Value dirName = root["dirName"];
		std::string dirNameS = dirName.asString();
		std::string new_dirId;
		int status;

		time_t now = time(0);
		char* dt = ctime(&now);
		std::string fecha(dt);

		Log(Log::LogMsgDebug) << "[" << "CreateDirNode " << "] userId: " << userId << " dirId: " << dirId << " Create directory " << dirNameS;
		if (this->rd->new_directory(userId, token, dirNameS, fecha, dirId, new_dirId, status)){
			conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			Log(Log::LogMsgDebug) << "[" << "CreateDirNode " << "] new dirId: " << new_dirId.c_str();
			conn.printfData("{\"dirId\": \"%s\"}",new_dirId.c_str());
		}else{
			Log(Log::LogMsgDebug) << "[" << "failed to create" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{\"dirId\": \"%s\"}","0");
		}
	}else{
			Log(Log::LogMsgDebug) << "[" << "failed to create" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{\"dirId\": \"%s\"}","0");
	}


}

void CreateDirNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}


