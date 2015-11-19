/*
 * delete_file_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "delete_file_node.h"
#include "info_node.h"

using std::string;
using std::stringstream;
using std::vector;

DeleteFileNode::DeleteFileNode(MgConnectionW&  conn)  : Node(conn) {
}

DeleteFileNode::~DeleteFileNode() {
}

vector<string> DeleteFileNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void DeleteFileNode::executeDelete() {
	vector<string> lista = DeleteFileNode::split(getUri(), '/');
	int status = 3;

	if ((!lista[3].compare("file")) && (lista.size() == 5)) {
		string userId = lista[2];
		string fileId = lista[4];

		Log(Log::LogMsgDebug) << "[" << "DeleteFileNode " << "] userId: " << userId << " fileId: " << fileId;
		RequestDispatcher::file_info_st file_info;

		if (!getRequestDispatcher()->get_file_info(userId,fileId,file_info,status)){

			Log(Log::LogMsgDebug) << "[" << "DeleteFileNode " << "] fail recover file info ";
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		}else{
			std::string parentDir=file_info.parent_directory;
			if (!getRequestDispatcher()->delete_file(userId,fileId,status)) {
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				string msg = handlerError(status);
				getConnection().printfData(msg.c_str());
			}else{
				Log(Log::LogMsgDebug) << "[DeleteFileNode], file deleted, parent folder printing, id: "<<parentDir;
				MgConnectionW mg=getConnection();
				std::string uri;
				uri = "/info/users/"+ userId + "/dir/" + parentDir;
				mg.setMethod("GET");
				mg.setUri(uri);

				Log(Log::LogMsgDebug) << "[DeleteFileNode] Uri: " <<uri;
				InfoNode* in=new InfoNode(mg);
				in->setRequestDispatcher(RequestDispatcher::get_instance());

				in->executeGet();
				delete in;
			}
		}
	}else{
		status=11;
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}

std::string DeleteFileNode::defaultResponse(){
    return "{\"resultCode\": \"2\"}";
}

std::string DeleteFileNode::getUserId() {
	vector<string> lista=DeleteFileNode::split(getUri(),'/');
	return lista[2];
}


