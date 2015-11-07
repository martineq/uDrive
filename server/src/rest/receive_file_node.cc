/*
 * receive_file_node.h
 *
 *  Created on: 26/10/2015
 *      Author: martin
 */

#include "receive_file_node.h"
#include "info_node.h"

using std::string;
using std::stringstream;

ReceiveFileNode::ReceiveFileNode() : Node("ReceiveFileNode") {
}

ReceiveFileNode::~ReceiveFileNode() {
}
vector<string> ReceiveFileNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ReceiveFileNode::executePost(MgConnectionW& conn, const char* url){
	vector<string> lista=ReceiveFileNode::split(url,'/');
	Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] ";
	int status=11;

	if ( (!lista[3].compare("dir")) && (lista.size()==5)){
		std::string userId=lista[2];
		std::string dirId=lista[4];
		std::string file_id;
		std::string token=conn.getAuthorization();
		std::string p_file;
		std::string p_file_aux;

		Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] token: " << token << " UserID: " << userId;
		Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] userId: " << userId << " dirId: " << dirId;
		std::string variable;
		std::string contenido;
		std::string nombre_archivo;

		time_t now = time(0);
		char* dt = ctime(&now);
		std::string fecha(dt);

		while((p_file_aux = conn.getMultipartData(variable, contenido)) != ""){
			if (variable == "file") {
				Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "], Contenido de file: --";
				p_file=p_file_aux;
			}
			if(variable == "fileName") {
				Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "], Contenido de fileName: " << p_file_aux;
				nombre_archivo=p_file;
			}
		}
		Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], multipart receive finished";
		if(!this->rd->new_file(userId, /*token,*/ nombre_archivo, ".jpg",fecha, p_file.c_str(),"8",dirId,file_id,status)){ //TODO(martindonofrio): use RequestDispatcher::check_token()
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			conn.printfData(msg.c_str());
		}else{
			Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], file accepted, parent folder printing";
			InfoNode * in=new InfoNode();
			in->setRequestDispatcher(RequestDispatcher::get_instance("db_test",9999));
			std::string uri;
			uri = "/info/users/"+ userId + "/dir/" + dirId;
			conn->request_method="GET";
			in->execute(conn,uri.c_str());
		}
	}else{
		status=11;
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		conn.printfData(msg.c_str());
	}
}
void ReceiveFileNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}

std::string ReceiveFileNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\"}]";
}
