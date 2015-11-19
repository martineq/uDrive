/*
 * list_tags_user_node.cc
 *
 *  Created on: 16/11/2015
 *      Author: Martin
 */

#include "list_tags_user_node.h"

using std::string;
using std::stringstream;
using std::vector;

ListTagsUserNode::ListTagsUserNode(MgConnectionW&  conn)  : Node(conn) {
}

ListTagsUserNode::~ListTagsUserNode() {
}

vector<string> ListTagsUserNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ListTagsUserNode::executeGet() {
	vector<string> lista=ListTagsUserNode::split(getUri(),'/');

	int status=11;
	Log(Log::LogMsgDebug) << "[ListTagsUserNode]";

	if ( (lista.size()==4)){
		string userId=getUserId();

		Log(Log::LogMsgDebug) << "[ListTagsUserNode], UserId: " <<userId;

		vector<string> listTags;
		std::string tags="";

		if (!getRequestDispatcher()->get_tags(userId,listTags,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
				Log(Log::LogMsgDebug) << "[ListTagsUserNode], listing -- Number of items: " << listTags.size();
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);

				if (listTags.size()!=0){
						for (int index = 0; index < listTags.size() - 1; ++index) {
							Log(Log::LogMsgDebug) << "[ListTagsUserNode], TAG: " << listTags[index];
							tags = tags + listTags[index] + ";";
						}
						tags = tags + listTags[listTags.size() - 1];
					}
				}
				std::string msg="{\"tags\":\"" + tags + "\"}";
				getConnection().printfData(msg.c_str());
	}else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string ListTagsUserNode::defaultResponse(){
	return "[{\"tagName\": \"\"}]";
}

std::string ListTagsUserNode::getUserId(){
	vector<string> lista=ListTagsUserNode::split(getUri(),'/');
	return lista[3];
}

