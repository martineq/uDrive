/*
 * delete_file_trash_node.cc
 *
 *  Created on: 15/11/2015
 *      Author: Martin
 */

#include "delete_file_trash_node.h"
#include "info_trash_node.h"

using std::string;
using std::vector;

DeleteFileTrashNode::DeleteFileTrashNode(MgConnectionW&  conn)  : Node(conn) {
}

DeleteFileTrashNode::~DeleteFileTrashNode() {
}

vector<string> DeleteFileTrashNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void DeleteFileTrashNode::executeDelete() {
	vector<string> lista=DeleteFileTrashNode::split(getUri(),'/');
	string dirId="";
	int status=11;
	if ( (!lista[4].compare("trash")) && (lista.size()==5)){
		string userId=getUserId();
		Log(Log::LogMsgDebug) << "[DeleteFileTrashNode], UserId: " <<userId;

		string filesParams=getConnection().getParameter("fileIds");
		Log(Log::LogMsgDebug) << "[DeleteFileTrashNode], Parameters: " << filesParams;
		vector<std::string> selected_files=DeleteFileTrashNode::split(filesParams,',');

		if (!getRequestDispatcher()->purge_deleted_files(userId,selected_files,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
			MgConnectionW mg=getConnection();
			InfoTrashNode * itn=new InfoTrashNode(mg);
			itn->setRequestDispatcher(RequestDispatcher::get_instance());
			std::string uri;
			uri = "/info/users/"+ userId + "/trash";
			mg.setMethod("GET");
			mg.setUri(uri);
			itn->executeGet();
			delete itn;
		}
	}
	else{
		Log(Log::LogMsgDebug) << "[DeleteFileTrashNode], URL: "<<getUri();
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}

std::string DeleteFileTrashNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\", \"userOwner \":\"0\"}]";
}

std::string DeleteFileTrashNode::getUserId(){
	vector<string> lista=DeleteFileTrashNode::split(getUri(),'/');
	return lista[3];
}

