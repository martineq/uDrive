/*
 * search_owner_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "search_owner_node.h"

using std::string;
using std::stringstream;
using std::vector;

SearchOwnerNode::SearchOwnerNode(MgConnectionW&  conn)  : Node(conn) {
}

SearchOwnerNode::~SearchOwnerNode() {
}

vector<string> SearchOwnerNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void SearchOwnerNode::executeGet() {
	vector<string> lista=SearchOwnerNode::split(getUri(),'/');
	int status=11;
	if ( (!lista[4].compare("owners"))){
		Log(Log::LogMsgDebug) << "[SearchOwnerNode]";
		string userId=getUserId();
		string owner=lista[5];
		bool result=false;
		Log(Log::LogMsgDebug) << "[SearchOwnerNode], UserId: " <<userId<< ", owner: " << owner;

		std::ostringstream item;
		vector<RequestDispatcher::info_element_st> elem_info;
		string lastVersion="1";

		if (getRequestDispatcher()->search_by_user(userId,owner, elem_info,status)){
			item << "[";
			Log(Log::LogMsgDebug) << "[SearchOwnerNode]: listing element "<< elem_info.size();
			if (elem_info.size()!=0) {
				for (int i = 0; i < elem_info.size() - 1; ++i) {
					item
					<< "{\"id\":\"" << (elem_info[i]).id << "\","
					<< "\"name\":\"" << (elem_info[i]).name << "\","
					<< "\"size\":\"" << (elem_info[i]).size << "\","
					<< "\"type\":\"" << (elem_info[i]).type << "\","
					<< "\"cantItems\":\"" << (elem_info[i]).number_of_items << "\","
					<< "\"shared\":\"" << (elem_info[i]).shared << "\", "
					<< "\"lastModDate\":\"" << (elem_info[i]).lastModDate << "\", "
					<< "\"userOwner\":\"" << (elem_info[i]).owner << "\", "
					<< "\"lastVersion\":\"" << lastVersion << "\"},";
					result = true;
				}
				Log(Log::LogMsgDebug) << "[SearchOwnerNode]: " << elem_info.size();
				int i = elem_info.size() - 1;
				item
				<< "{\"id\":\"" << (elem_info[i]).id << "\","
				<< "\"name\":\"" << (elem_info[i]).name << "\","
				<< "\"size\":\"" << (elem_info[i]).size << "\","
				<< "\"type\":\"" << (elem_info[i]).type << "\","
				<< "\"cantItems\":\"" << (elem_info[i]).number_of_items << "\","
				<< "\"shared\":\"" << (elem_info[i]).shared << "\", "
				<< "\"lastModDate\":\"" << (elem_info[i]).lastModDate << "\", "
				<< "\"userOwner\":\"" << (elem_info[i]).owner << "\", "
				<< "\"lastVersion\":\"" << lastVersion << "\"}";
				result = true;
				Log(Log::LogMsgDebug) << "[SearchOwnerNode]: ";
			}else{
				item << "]";
				result = true;
			}

		}

		if (!result) {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		} else {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);

			const std::string tmp = item.str();
			const char* msg = tmp.c_str();
			Log(Log::LogMsgDebug) << tmp.c_str();
			getConnection().printfData(tmp.c_str());
		}
	}
	else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string SearchOwnerNode::defaultResponse(){
	return "[]";
}

std::string SearchOwnerNode::getUserId(){
	vector<string> lista=SearchOwnerNode::split(getUri(),'/');
	return lista[3];
}

