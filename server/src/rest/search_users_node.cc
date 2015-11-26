/*
 * search_users_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "search_users_node.h"

using std::string;
using std::stringstream;
using std::vector;

SearchUsersNode::SearchUsersNode(MgConnectionW&  conn)  : Node(conn) {
}

SearchUsersNode::~SearchUsersNode() {
}

vector<string> SearchUsersNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void SearchUsersNode::executeGet() {
	vector<string> lista=SearchUsersNode::split(getUri(),'/');
	string fileId ="";
	int status=0;
	if ( (!lista[3].compare("users"))){
		Log(Log::LogMsgDebug) << "[SearchUsersNode]";
		string userId=getUserId();
		bool result=false;

		string query_mail = getConnection().getParameter("q");
		std::ostringstream item;
		item << "[";
		vector<RequestDispatcher::user_info_st> listaUsersInfo;

		if (query_mail=="%40") query_mail="@";

		Log(Log::LogMsgDebug) << "[SearchUsersNode], UserId: " <<userId << " Patron: " <<query_mail;

		if (getRequestDispatcher()->get_user_email_list_by_pattern(query_mail,listaUsersInfo,status)){
			Log(Log::LogMsgDebug) << "[SearchUsersNode]: list users , size: ";
			if (listaUsersInfo.size()!=0) {
				for (int i = 0; i < listaUsersInfo.size()-1 ; ++i) {
					item
					<< "{\"id\":\"" << listaUsersInfo[i].id << "\","
					<< "\"firstName\":\"" << listaUsersInfo[i].first_name << "\","
					<< "\"lastName\":\"" << listaUsersInfo[i].last_name << "\","
					<< "\"email\":\"" << listaUsersInfo[i].email << "\"},";
					result=true;
				}
				Log(Log::LogMsgDebug) << "[SearchUsersNode]: "<<listaUsersInfo.size();
				item
				<< "{\"id\":\"" << listaUsersInfo[listaUsersInfo.size()-1].id << "\","
				<< "\"firstName\":\"" << listaUsersInfo[listaUsersInfo.size()-1].first_name << "\","
				<< "\"lastName\":\"" << listaUsersInfo[listaUsersInfo.size()-1].last_name << "\","
				<< "\"email\":\"" << listaUsersInfo[listaUsersInfo.size()-1].email << "\"}";
				result=true;
				Log(Log::LogMsgDebug) << "[SearchUsersNode]: last json added.";
			}else{
				result = true;
			}
		}
		item << "]";

		if (!result) {
			Log(Log::LogMsgDebug) << "[SearchUsersNode]: not result";
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		} else {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);

			const std::string tmp = item.str();
			Log(Log::LogMsgDebug) << tmp;
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
std::string SearchUsersNode::defaultResponse(){
	return "[]";
}

std::string SearchUsersNode::getUserId(){
	vector<string> lista=SearchUsersNode::split(getUri(),'/');
	return lista[2];
}

