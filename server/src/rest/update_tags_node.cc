/*
 * update_tags_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "update_tags_node.h"

using std::string;
using std::stringstream;
using std::vector;

UpdateTagsNode::UpdateTagsNode(MgConnectionW&  conn)  : Node(conn) {
}

UpdateTagsNode::~UpdateTagsNode() {
}

vector<string> UpdateTagsNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void UpdateTagsNode::executePut() {
	vector<string> lista=UpdateTagsNode::split(getUri(),'/');
	string id ="";
	int status=11;
	Log(Log::LogMsgDebug) << "[UpdateTagsNode]";
	std:string tags="";

	if ( ( (!lista[4].compare("file")) or (!lista[4].compare("dir"))) && (lista.size()==6)){
		Log(Log::LogMsgDebug) << "[UpdateTagsNode]";
		string userId=getUserId();
		id =lista[5];
		bool result=false;
		RequestDispatcher::dir_info_st dir_info;
		RequestDispatcher::file_info_st file_info;

		Log(Log::LogMsgDebug) << "[ListTagsNode], UserId: " <<userId<< ", Id: " << id;

		tags=getConnection().getBodyJson("tags");

		Log(Log::LogMsgDebug) << "[ListTagsNode], Tags: "<<tags;

		if (lista[4]=="file") result=getRequestDispatcher()->get_file_info(userId,id,file_info,status);
		else result=getRequestDispatcher()->get_directory_info(userId,id,dir_info,status);

		if (!result){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());

		}else if ( (!lista[4].compare("file")) and (!getRequestDispatcher()->modify_file_info(userId,
			id,file_info.name,file_info.extension,file_info.date_last_mod,tags,status)) ){

			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else if ( (!lista[4].compare("dir")) and (!getRequestDispatcher()->modify_directory_info(userId,
			id,dir_info.name,dir_info.date_last_mod,tags,status)) ){

			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else{
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			Log(Log::LogMsgDebug) << "[UpdateTagsNode] resultCode: 1";
			getConnection().printfData("{\"resultCode\": \"1\"}");
		}
	}
	else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string UpdateTagsNode::defaultResponse(){
	return "{\"resultCode\": \"2\"}";
}

std::string UpdateTagsNode::getUserId(){
	vector<string> lista=UpdateTagsNode::split(getUri(),'/');
	return lista[3];
}

