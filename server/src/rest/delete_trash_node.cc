/*
 * delete_trash_node.cc
 *
 *  Created on: 15/11/2015
 *      Author: Martin
 */

#include "delete_trash_node.h"
#include "info_trash_node.h"
using std::string;
using std::vector;

DeleteTrashNode::DeleteTrashNode(MgConnectionW&  conn)  : Node(conn) {
}

DeleteTrashNode::~DeleteTrashNode() {
}

vector<string> DeleteTrashNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void DeleteTrashNode::executeDelete() {
	vector<string> lista=DeleteTrashNode::split(getUri(),'/');
	string dirId="";
	int status=11;

	if ( (!lista[4].compare("trash")) && (lista.size()==5)){
		string userId=getUserId();
		Log(Log::LogMsgDebug) << "[DeleteTrashNode], UserId: " <<userId;

		vector<RequestDispatcher::info_element_st> deleted_files;

		if (!getRequestDispatcher()->purge_deleted_files(userId,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else {
			Log(Log::LogMsgDebug) << "[DeleteTrashNode], URL: "<<getUri();
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg="[]";
			getConnection().printfData(msg.c_str());
		}

	}
	else{
		Log(Log::LogMsgDebug) << "[DeleteTrashNode], URL: "<<getUri();
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}

std::string DeleteTrashNode::defaultResponse(){
	return "[]";
}

std::string DeleteTrashNode::getUserId(){
	vector<string> lista=DeleteTrashNode::split(getUri(),'/');
	return lista[3];
}

