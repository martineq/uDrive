/*
 * send_file_node.h
 *
 *  Created on: 26/10/2015
 *      Author: martin
 */

#include "send_file_node.h"
using std::string;

SendFileNode::SendFileNode(MgConnectionW&  conn)  : Node(conn) {
}

SendFileNode::~SendFileNode() {
}

void SendFileNode::executeGet() {
	vector<string> lista=SendFileNode::split(getConnection().getUri(),'/');
	Log(Log::LogMsgDebug) << "[" << "SendFileNode " << "] ";
	int status=11;

	if ( (!lista[3].compare("file")) && (lista.size()==5)){
		std::string userId=lista[2];
		std::string fileId=lista[4];

        std::string revision=getConnection().getParameter("version");

		char* p_file;
        std::string size="";

		Log(Log::LogMsgDebug) << "[" << "SendFileNode " << "] userId: " << userId << " dirId: " << fileId<< " Version: "<<revision;
        if (!getRequestDispatcher()->get_file_stream(userId,fileId,revision,p_file,size,status)  ){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else{
            Log(Log::LogMsgInfo) << "[" << "SendFileNode " << "], multipart send inited";
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::ContentTypes::CONTENT_TYPE_MULTIPART);
            getConnection().setMultipartData("file","dsa",p_file,atoi(size.c_str()));
		}
	}else{
		status=11;
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string SendFileNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\"}]";
}

vector<string> SendFileNode::split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}
std::string SendFileNode::getUserId() {
	vector<string> lista=SendFileNode::split(getUri(),'/');
	return lista[2];
}
