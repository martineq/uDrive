/*
 * delete_dir_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "delete_dir_node.h"

using std::string;
using std::stringstream;
using std::vector;

DeleteDirNode::DeleteDirNode(MgConnectionW&  conn)  : Node(conn) {
}

DeleteDirNode::~DeleteDirNode() {
}

vector<string> DeleteDirNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void DeleteDirNode::executeDelete() {
	vector<string> lista = DeleteDirNode::split(getUri(), '/');
	int status = 3;

	if ((!lista[3].compare("dir")) && (lista.size() == 5)) {
		string userId = lista[2];
		string dirId = lista[4];

		Log(Log::LogMsgDebug) << "[" << "DeleteDirNode " << "] userId: " << userId << " dirId: " << dirId;
		RequestDispatcher::dir_info_st dir_info;

		if (!getRequestDispatcher()->get_directory_info(userId, dirId,dir_info,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		}else Log(Log::LogMsgDebug) << "[DeleteDirNode] dirId parent: "<<dir_info.parent_directory;

		if (!getRequestDispatcher()->delete_directory(userId, dirId,status)) {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		}else{
			Log(Log::LogMsgInfo) << "[DeleteDirNode], dir erase, parent folder printing";
			MgConnectionW mg=getConnection();
			InfoNode* in=new InfoNode(mg);
			in->setRequestDispatcher(RequestDispatcher::get_instance());
			std::string uri;
			uri = "/info/users/"+ userId + "/dir/" + dir_info.parent_directory;
			mg.setMethod("GET");
			mg.setUri(uri);
			in->executeGet();
			delete in;
		}
	}
}

std::string DeleteDirNode::defaultResponse(){
    return "{\"resultCode\": \"2\"}";
}

std::string DeleteDirNode::getUserId() {
	vector<string> lista=DeleteDirNode::split(getUri(),'/');
	return lista[2];
}


