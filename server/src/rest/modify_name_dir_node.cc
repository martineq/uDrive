/*
 * modify_name_dir_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "modify_name_dir_node.h"
using std::string;
using std::stringstream;
using std::vector;

ModifyNameDirNode::ModifyNameDirNode(MgConnectionW&  conn)  : Node(conn) {
}

ModifyNameDirNode::~ModifyNameDirNode() {
}

vector<string> ModifyNameDirNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ModifyNameDirNode::executePut() {
	vector<string> lista=ModifyNameDirNode::split(getUri(),'/');
	int status=11;

	string msg="";
	if ( ( (!lista[4].compare("dir")) or (!lista[4].compare("file"))) and (lista.size()==6)){
		string userId=lista[3];
		string Id=lista[5];
		string token=getConnection().getAuthorization();
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;

		std::string dirNameS = getConnection().getBodyJson("dirName");
		bool succes=false;

		Log(Log::LogMsgDebug) << "[ModifyNameDirNode] userId: " << userId << " Id: " << Id << " Create directory " << dirNameS <<" "<<lista[4];

		if (lista[4]=="file") {
			RequestDispatcher::file_info_st file_info;
			if (!getRequestDispatcher()->get_file_info(userId,Id,file_info,status)) {
				msg="{\"resultCode\": \"2\"}";

			}else {
				if (!getRequestDispatcher()->modify_file_info(userId,Id,dirNameS,file_info.extension,file_info.date_last_mod,file_info.tags,status)) {
					msg="{\"resultCode\": \"2\"}";
				}else {
					msg="{\"resultCode\": \"1\"}";
					Log(Log::LogMsgDebug) << "[ModifyNameDirNode] file ok ";
					succes=true;
				}
			}
		}else {
			//dir
			RequestDispatcher::dir_info_st dir_info;
			if (!getRequestDispatcher()->get_directory_info(userId, Id, dir_info, status)) {
				msg = "{\"resultCode\": \"2\"}";
			} else {
				if (!getRequestDispatcher()->modify_directory_info(userId, Id, dirNameS,dir_info.date_last_mod,
																 dir_info.tags, status)) {
					msg = "{\"resultCode\": \"2\"}";
				} else {
					msg = "{\"resultCode\": \"1\"}";
					Log(Log::LogMsgDebug) << "[ModifyNameDirNode] dir ok ";
					succes = true;
				}
			}
		}

			if (succes){
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				Log(Log::LogMsgDebug) << "[ModifyNameDirNode] result ok: "<<msg;
				getConnection().printfData(msg.c_str());
			}else {
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				Log(Log::LogMsgDebug) << "[ModifyNameDirNode] result fail: "<<msg;
				getConnection().printfData(msg.c_str());
			}
	}else{
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
	}
}
std::string ModifyNameDirNode::defaultResponse(){
	return "[]";
}

std::string ModifyNameDirNode::getUserId() {
	vector<string> lista=ModifyNameDirNode::split(getUri(),'/');
	return lista[3];
}


