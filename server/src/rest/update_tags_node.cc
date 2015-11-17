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
	string fileId="";
	int status=11;
	Log(Log::LogMsgDebug) << "[UpdateTagsNode]";

	if ( (!lista[4].compare("files")) && (lista.size()==6)){

		Log(Log::LogMsgDebug) << "[UpdateTagsNode]";
		string userId=getUserId();
		fileId=lista[5];

		Log(Log::LogMsgDebug) << "[ListTagsNode], UserId: " <<userId<< ", FileId: " <<fileId;
		std:string tags=getConnection().getBodyJson("tags");

		Log(Log::LogMsgDebug) << "[ListTagsNode], Tags: "<<tags;

		RequestDispatcher::file_info_st file_info;

		if (!getRequestDispatcher()->get_file_info(userId,fileId,file_info,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());

		}else if (!getRequestDispatcher()->modify_file_info(userId,fileId,file_info.name,file_info.extension,file_info.date_last_mod,tags,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
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

