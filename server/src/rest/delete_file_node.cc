/*
 * delete_file_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "delete_file_node.h"
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
		Log(Log::LogMsgDebug) << "[" << "DeleteFileNode " << "] No implementado";

		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg = handlerError(status);
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

