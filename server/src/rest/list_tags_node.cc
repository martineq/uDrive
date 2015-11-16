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

	if ( (!lista[4].compare("files")) && (lista.size()==6)){
		string userId=getUserId();
		string fileId=lista[5];

		Log(Log::LogMsgDebug) << "[ListTagsNode], UserId: " <<userId<< ", FileId: " <<fileId;

		vector<string> listTags;
		std::string tags="";

		if (!getRequestDispatcher()->get_tags(userId,listTags,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
				Log(Log::LogMsgDebug) << "[" << "ListTagsNode" << "], listing directory -- Number of items: " << listTags.size();
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);

				for (int index = 0; index < listTags.size(); ++index) {
					Log(Log::LogMsgDebug) << "[ListTagsNode], TAG: " << listTags[index];
					tags = tags + listTags[index] + ";";
				}
				std::string msg="{\"tags\":\"" + tags + "\"}";
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
	return "[{\"tagName\": \"\"}]";
}

std::string ListTagsNode::getUserId(){
	vector<string> lista=ListTagsNode::split(getUri(),'/');
	return lista[3];
}

