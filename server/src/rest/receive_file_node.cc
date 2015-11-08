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

ReceiveFileNode::ReceiveFileNode(MgConnectionW&  conn)  : Node(conn) {
}

ReceiveFileNode::~ReceiveFileNode() {
}

void ReceiveFileNode::executePost() {
	vector<string> lista=ReceiveFileNode::split(getConnection().getUri(),'/');
	Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] ";
	int status=11;

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		std::string userId=lista[3];
		std::string dirId=lista[5];
		std::string file_id;
		std::string p_file;
		std::string p_file_aux;

		Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] userId: " << userId << " dirId: " << dirId;
		std::string variable;
		std::string contenido;
		std::string nombre_archivo;
		std::string extension="";
		std::string size="";

		time_t now = time(0);
		char* dt = ctime(&now);
		std::string fecha(dt);

		while((p_file_aux = getConnection().getMultipartData(variable, contenido)) != ""){
			if (variable == "file") {
				Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "], Contenido de file: ...";
				p_file=p_file_aux;
                break;
			}
			if(variable == "fileName") {
				Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "], Contenido de fileName: " << p_file_aux;
				nombre_archivo=p_file;
			}
			if(variable == "extension") {
				Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "], Contenido de extension: " << p_file_aux;
				nombre_archivo=p_file;
			}
			if(variable == "size") {
				Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "], Contenido de size: " << p_file_aux;
				nombre_archivo=p_file;
			}
		}

		Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], multipart receive finished";
		if(!getRequestDispatcher()->new_file(userId,nombre_archivo, extension,fecha, p_file.c_str(), size,dirId,file_id,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else{
			Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], file accepted, parent folder printing";
			MgConnectionW mg=getConnection();
			InfoNode* in=new InfoNode(mg);
			in->setRequestDispatcher(RequestDispatcher::get_instance("db_test",9999));
			std::string uri;
			uri = "/info/users/"+ userId + "/dir/" + dirId;
			getConnection().setMethod("GET");
			getConnection().setUri(uri);
			in->execute();
		}
	}else{
		status=11;
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string ReceiveFileNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\"}]";
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
std::string ReceiveFileNode::getUserId() {
	vector<string> lista=ReceiveFileNode::split(getUri(),'/');
	return lista[3];
}
