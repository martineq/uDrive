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

		Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "] userId: " << userId << " dirId: " << dirId;
		std::string variable="";
		std::string contenido="";
		std::string extension="";
		std::string size="";

		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];
		time (&rawtime);
		timeinfo = localtime (&rawtime);
		strftime (buffer,80," %d/%m/%Y %X",timeinfo);

		Log(Log::LogMsgDebug) << "[ReceiveFileNode]: fecha: "<<buffer;
		std::string fecha(buffer);

        p_file = getConnection().getMultipartData(variable, contenido);
		while(p_file!= "") {
            if (variable == "file") {
                Log(Log::LogMsgDebug) << "[" << "ReceiveFileNode " << "]: Variable: "<<variable<< ", Nombre de archivo: " << contenido; //<< ", p_file_aux: " << p_file;
                break;
            };
            p_file = getConnection().getMultipartData(variable, contenido);
        }
        Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], multipart receive finished";
        vector<string> partesArchivo=ReceiveFileNode::split(contenido,'.');
        Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], Nombre Archivo: "<<partesArchivo[0] << ", Extension: "<<partesArchivo[1];
        Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], Tamaño de Archivo:  "<<p_file.size();

        stringstream tamanio;
        tamanio << p_file.size();

        if ( (p_file.size() <= 0) or (!getRequestDispatcher()->new_file(userId,contenido, partesArchivo[1],fecha, p_file.c_str(), tamanio.str(),dirId,file_id,status)) ){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}else{
			Log(Log::LogMsgInfo) << "[" << "ReceiveFileNode " << "], file accepted, parent folder printing";
			MgConnectionW mg=getConnection();
			InfoNode* in=new InfoNode(mg);
            in->setRequestDispatcher(RequestDispatcher::get_instance());
			std::string uri;
			uri = "/info/users/"+ userId + "/dir/" + dirId;
            mg.setMethod("GET");
            mg.setUri(uri);
			in->executeGet();
            delete in;
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
