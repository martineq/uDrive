/*
 * list_tags_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "list_tags_node.h"

using std::string;
using std::stringstream;
using std::vector;

ListTagsNode::ListTagsNode(MgConnectionW&  conn)  : Node(conn) {
}

ListTagsNode::~ListTagsNode() {
}

vector<string> ListTagsNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ListTagsNode::executeGet() {
	vector<string> lista=ListTagsNode::split(getUri(),'/');

	int status=11;
	Log(Log::LogMsgDebug) << "[ListTagsNode]";

	if ( ( (!lista[4].compare("file")) or (!lista[4].compare("dir"))) && (lista.size()==6)){
		string userId=getUserId();
		string id =lista[5];

		Log(Log::LogMsgDebug) << "[ListTagsNode], UserId: " <<userId<< ", Id: " << id;

		string listTags="";
		bool result=false;

		RequestDispatcher::file_info_st file_info;
		if (lista[4]=="file"){
			RequestDispatcher::file_info_st file_info;
			result=getRequestDispatcher()->get_file_info(userId,id,file_info,status);
			listTags=file_info.tags;
		}else{
			RequestDispatcher::dir_info_st dir_info;
			result=getRequestDispatcher()->get_directory_info(userId,id,dir_info,status);
			listTags=dir_info.tags;
		}

		if (!result){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
				Log(Log::LogMsgDebug) << "[ListTagsNode]";
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				std::string msg="{\"tags\":\"" + listTags + "\"}";
				getConnection().printfData(msg.c_str());
			}

	}else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string ListTagsNode::defaultResponse(){
	return "[{\"tags\": \"\"}]";
}

std::string ListTagsNode::getUserId(){
	vector<string> lista=ListTagsNode::split(getUri(),'/');
	return lista[3];
}

