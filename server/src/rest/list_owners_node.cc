/*
 * list_owners_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "list_owners_node.h"

using std::string;
using std::stringstream;
using std::vector;

ListOwnersNode::ListOwnersNode(MgConnectionW&  conn)  : Node(conn) {
}

ListOwnersNode::~ListOwnersNode() {
}

vector<string> ListOwnersNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ListOwnersNode::executeGet() {
	vector<string> lista=ListOwnersNode::split(getUri(),'/');
	string fileId ="";
	int status=11;
	if (lista.size()==4){
		Log(Log::LogMsgDebug) << "[ListOwnersNode]";
		string userId=getUserId();
		bool result=false;

		Log(Log::LogMsgDebug) << "[ListOwnersNode], UserId: " <<userId;
		std::ostringstream item;

		vector<RequestDispatcher::user_info_st> lista_user_info;
		item << "[";
		if (getRequestDispatcher()->get_owners_of_shared_files(userId,lista_user_info,status)){
			Log(Log::LogMsgDebug) << "[ListOwnersNode]: list owners users " << lista_user_info.size();
			if (lista_user_info.size()!=0) {
				for (int i = 0; i < lista_user_info.size() - 1; ++i) {
					item
					<< "{\"id\":\"" << lista_user_info[i].id << "\","
					<< "\"firstName\":\"" << lista_user_info[i].first_name << "\","
					<< "\"lastName\":\"" << lista_user_info[i].last_name << "\","
					<< "\"email\":\"" << lista_user_info[i].email << "\"},";
					result = true;
				}
				item
				<< "{\"id\":\"" << lista_user_info[lista_user_info.size() - 1].id << "\","
				<< "\"firstName\":\"" << lista_user_info[lista_user_info.size() - 1].first_name << "\","
				<< "\"lastName\":\"" << lista_user_info[lista_user_info.size() - 1].last_name << "\","
				<< "\"email\":\"" << lista_user_info[lista_user_info.size() - 1].email << "\"}";
				result = true;
				Log(Log::LogMsgDebug) << "[ListOwnersNode] last node added ";
			}else{
				result = true;
			}
		}
		item << "]";

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
std::string ListOwnersNode::defaultResponse(){
	return "[]";
}

std::string ListOwnersNode::getUserId(){
	vector<string> lista=ListOwnersNode::split(getUri(),'/');
	return lista[3];
}

