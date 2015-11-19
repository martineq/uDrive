/*
 * update_collaborators_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "update_collaborators_node.h"

using std::string;
using std::stringstream;
using std::vector;

UpdateCollaboratorsNode::UpdateCollaboratorsNode(MgConnectionW&  conn)  : Node(conn) {
}

UpdateCollaboratorsNode::~UpdateCollaboratorsNode() {
}

vector<string> UpdateCollaboratorsNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void UpdateCollaboratorsNode::executePost() {
	vector<string> lista = UpdateCollaboratorsNode::split(getUri(), '/');
	string fileId = "";
	int status = 0;

	Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode] " <<lista[4];
	Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode] " <<lista[6];

	if ( (lista[4]=="files") and (lista[6]=="collaborators") ) {
		Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode]";
		string userId = getUserId();
		fileId = lista[5];
		bool result = false;

		Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode], UserId: " << userId << ", fileId: " << fileId;
		const Json::Value root = getConnection().getBodyJson();

		time_t rawtime;
		struct tm *timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, " %d/%m/%Y %X", timeinfo);

		Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode]: fecha: " << buffer;
		std::string fecha(buffer);
		std::ostringstream item;
		item << "[";

		RequestDispatcher::user_info_st user_share_info;
		vector<std::string> listaUserShared;

		Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode]: json size: " <<root.size();

		for (int i = 0; i < root.size(); ++i) {
			Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode]: Id nodo: " <<root[i]["id"].asString();
			listaUserShared.push_back(root[i]["id"].asString());
		};

		Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode]: lista users size: " << listaUserShared.size();

		if (getRequestDispatcher()->overwrite_file_sharing_by_list(userId, fileId, listaUserShared, fecha, status)) {
			Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode]: Cant Collaborators: "<<listaUserShared.size();
			if (listaUserShared.size() > 0) {
				for (int i = 0; i < listaUserShared.size() - 1; ++i) {
					if (getRequestDispatcher()->get_user_info(listaUserShared[i], user_share_info, status)) {
						item
						<< "{\"id\":\"" << listaUserShared[i] << "\","
						<< "\"firstName\":\"" << user_share_info.first_name << "\","
						<< "\"lastName\":\"" << user_share_info.last_name << "\","
						<< "\"email\":\"" << user_share_info.email << "\"},";
						result = true;
					}
				}
				if (getRequestDispatcher()->get_user_info(listaUserShared[listaUserShared.size()-1], user_share_info,status)) {
					item
					<< "{\"id\":\"" << listaUserShared[listaUserShared.size()-1] << "\","
					<< "\"firstName\":\"" << user_share_info.first_name << "\","
					<< "\"lastName\":\"" << user_share_info.last_name << "\","
					<< "\"email\":\"" << user_share_info.email << "\"}";
					result = true;
				}
			} else result = true;
		}
		item << "]";

		if (!result) {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		} else {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);

			const std::string tmp = item.str();
			const char *msg = tmp.c_str();
			Log(Log::LogMsgDebug) << "[UpdateCollaboratorsNode] msg: " <<msg;
			getConnection().printfData(msg);
		}
	}else {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		}
}
std::string UpdateCollaboratorsNode::defaultResponse(){
	return "[]";
}

std::string UpdateCollaboratorsNode::getUserId(){
	vector<string> lista=UpdateCollaboratorsNode::split(getUri(),'/');
	return lista[3];
}

