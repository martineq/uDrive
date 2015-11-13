/*
 * send_dir_node.h
 *
 *  Created on: 12/11/2015
 *      Author: martin
 */

#include "send_dir_node.h"
using std::string;

SendDirNode::SendDirNode(MgConnectionW&  conn)  : Node(conn) {
}

SendDirNode::~SendDirNode() {
}

void SendDirNode::executeGet() {
	vector<string> lista=SendDirNode::split(getConnection().getUri(),'/');
	Log(Log::LogMsgDebug) << "[" << "SendDirNode " << "] ";
	int status=11;

	if ( (!lista[3].compare("file")) && (lista.size()==5)){
		std::string userId=lista[2];
		std::string dirId=lista[4];
        std::string revision="1";
		char* p_file;
        std::string size="";

		Log(Log::LogMsgDebug) << "[" << "SendDirNode " << "] userId: " << userId << " dirId: " << dirId;
        if (!getRequestDispatcher()->get_dir_stream(userId,dirId,p_file,size,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else{
            Log(Log::LogMsgInfo) << "[" << "SendDirNode " << "], multipart send inited";
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::ContentTypes::CONTENT_TYPE_MULTIPART);
            getConnection().setMultipartData("file","",p_file,atoi(size.c_str()));
		}
	}else{
		status=11;
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string SendDirNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\"}]";
}

vector<string> SendDirNode::split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}
std::string SendDirNode::getUserId() {
	vector<string> lista=SendDirNode::split(getUri(),'/');
	return lista[2];
}
