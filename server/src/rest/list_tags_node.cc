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
	string dirId="";
	int status=11;
	Log(Log::LogMsgDebug) << "[ListTagsNode]";

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		string userId=getUserId();
		dirId=lista[5];

		Log(Log::LogMsgDebug) << "[ListTagsNode], UserId: " <<userId;

		vector<string> listTags;

		if (!getRequestDispatcher()->get_tags(userId,listTags,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
			bool enc = false;
			std::ostringstream item;
  			item << "[";
			vector<std::string>::iterator listTags_it;
			Log(Log::LogMsgDebug) << "[ListTagsNode], touring list of tags";
			if (listTags.size()!=0){
				for (listTags_it = listTags.begin(); listTags_it < (listTags.end()-1); listTags_it++){
					enc=true;
					item
					<< "{\"tagName\":\"" << (*listTags_it) << "\",";
				}
			}
			if (listTags.size()==1) enc=true;
			if (!enc){
				//empty dir
				Log(Log::LogMsgDebug) << "[" << "ListTagsNode" << "] -- Empty dir.";
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				getConnection().printfData(defaultResponse().c_str());
			}else{
				item
				<< "{\"tagName\":\"" << (*listTags_it) << "\"}";
				item << "]";
				Log(Log::LogMsgDebug) << "[" << "ListTagsNode" << "], listing directory -- Number of items: " << listTags.size();
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				const std::string tmp = item.str();
				const char* msg = tmp.c_str();
				getConnection().printfData(msg);
			}

		}
	}
	else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string ListTagsNode::defaultResponse(){
	return "[{\"tagName\": \"\"}]";
}

std::string ListTagsNode::getUserId(){
	vector<string> lista=ListTagsNode::split(getUri(),'/');
	return lista[3];
}

