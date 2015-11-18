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
	int status=11;
	if ( (!lista[4].compare("file"))){
		Log(Log::LogMsgDebug) << "[SearchUsersNode]";
		string userId=getUserId();
		fileId =lista[5];
		bool result=false;

		Log(Log::LogMsgDebug) << "[SearchUsersNode], UserId: " <<userId<< ", fileId: " << fileId;
		const string query_mail = getConnection().getParameter("q");

		std::ostringstream item;
		item << "[";
		vector<RequestDispatcher::user_info_st> listaUsersInfo;

		//TODO (martin): Modificar este vector, descomentando el de arriba una vez que mart libere el cambio.
		//if (getRequestDispatcher()->get_user_email_list_by_pattern(query_mail,listaUsersInfo,status)){
		if (true){
			Log(Log::LogMsgDebug) << "[SearchUsersNode]: list users ";
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
std::string SearchUsersNode::defaultResponse(){
	return "[]";
}

std::string SearchUsersNode::getUserId(){
	vector<string> lista=SearchUsersNode::split(getUri(),'/');
	return lista[3];
}

