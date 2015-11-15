/*
 * restore_trash_node.cc
 *
 *  Created on: 15/11/2015
 *      Author: Martin
 */

#include "restore_trash_node.h"
#include "info_trash_node.h"
using std::string;
using std::vector;

RestoreTrashNode::RestoreTrashNode(MgConnectionW&  conn)  : Node(conn) {
}

RestoreTrashNode::~RestoreTrashNode() {
}

vector<string> RestoreTrashNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void RestoreTrashNode::executeDelete() {
	vector<string> lista=RestoreTrashNode::split(getUri(),'/');
	string dirId="";
	int status=11;

	if ( (!lista[4].compare("trash")) && (lista.size()==6)){
		string userId=getUserId();
		Log(Log::LogMsgDebug) << "[RestoreTrashNode], UserId: " <<userId;

		vector<RequestDispatcher::info_element_st> deleted_files;

		if (!getRequestDispatcher()->recover_deleted_files(userId,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else {
			MgConnectionW mg=getConnection();
			mg.setMethod("GET");
			InfoTrashNode * itn=new InfoTrashNode(mg);
			itn->setRequestDispatcher(RequestDispatcher::get_instance());
			itn->executeGet();
			delete itn;
		}

	}
	else{
		Log(Log::LogMsgDebug) << "[RestoreTrashNode], URL: "<<getUri();
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}

std::string RestoreTrashNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\", \"userOwner \":\"0\"}]";
}

std::string RestoreTrashNode::getUserId(){
	vector<string> lista=RestoreTrashNode::split(getUri(),'/');
	return lista[3];
}

