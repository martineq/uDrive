/*
 * list_revisions_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "list_revisions_node.h"

using std::string;
using std::stringstream;
using std::vector;

ListRevisionsNode::ListRevisionsNode(MgConnectionW&  conn)  : Node(conn) {
}

ListRevisionsNode::~ListRevisionsNode() {
}

vector<string> ListRevisionsNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ListRevisionsNode::executeGet() {
	vector<string> lista=ListRevisionsNode::split(getUri(),'/');
	string fileId ="";
	int status=11;
	if ( (!lista[4].compare("file"))){
		Log(Log::LogMsgDebug) << "[ListRevisionsNode]";
		string userId=getUserId();
		fileId =lista[5];
		bool result=false;

		Log(Log::LogMsgDebug) << "[ListRevisionsNode], UserId: " <<userId<< ", fileId: " << fileId;

		std::ostringstream item;
		item << "[";

		RequestDispatcher::user_info_st user_info;
		vector<RequestDispatcher::user_info_st> lista_user_info;

		//TODO (martindonofrio): ver que metodo usar para mandar el listado de revisiones.
		if (true){
			Log(Log::LogMsgDebug) << "[ListRevisionsNode]: list colaborators users ";
			for (int i = 0; i < lista_user_info.size()-1 ; ++i) {
				item
				<< "{\"id\":\"" << lista_user_info[i].id << "\","
				<< "\"firstName\":\"" << lista_user_info[i].first_name << "\","
				<< "\"lastName\":\"" << lista_user_info[i].last_name << "\","
				<< "\"email\":\"" << lista_user_info[i].email << "\"},";
				result=true;
			}
			Log(Log::LogMsgDebug) << "[ListRevisionsNode]: "<<lista_user_info.size();
			item
			<< "{\"id\":\"" << lista_user_info[lista_user_info.size()-1].id << "\","
			<< "\"firstName\":\"" << lista_user_info[lista_user_info.size()-1].first_name << "\","
			<< "\"lastName\":\"" << lista_user_info[lista_user_info.size()-1].last_name << "\","
			<< "\"email\":\"" << lista_user_info[lista_user_info.size()-1].email << "\"}";
			result=true;
			Log(Log::LogMsgDebug) << "[ListRevisionsNode]: ";

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
std::string ListRevisionsNode::defaultResponse(){
	return "[]";
}

std::string ListRevisionsNode::getUserId(){
	vector<string> lista=ListRevisionsNode::split(getUri(),'/');
	return lista[3];
}

