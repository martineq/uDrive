/*
 * list_info_elem_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "list_info_elem_node.h"

using std::string;
using std::stringstream;
using std::vector;

ListInfoElemNode::ListInfoElemNode(MgConnectionW&  conn)  : Node(conn) {
}

ListInfoElemNode::~ListInfoElemNode() {
}

vector<string> ListInfoElemNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void ListInfoElemNode::executeGet() {
	vector<string> lista=ListInfoElemNode::split(getUri(),'/');
	string Id ="";
	int status=11;
	if (lista[4].compare("file") or  (lista[4].compare("dir"))) {
		Log(Log::LogMsgDebug) << "[ListInfoElemNode]";
		string userId=getUserId();
		Id =lista[5];
		bool result=false;
		Log(Log::LogMsgDebug) << "[ListInfoElemNode], UserId: " <<userId<< ", " << lista[4] <<": "<<Id;
		std::ostringstream item;
		item << "{";

		if (lista[5].compare("file")){
			Log(Log::LogMsgDebug) << "[ListInfoElemNode], File. ";
			RequestDispatcher::file_info_st file_info;
			if (getRequestDispatcher()->get_file_info(userId,Id,file_info,status)){
				Log(Log::LogMsgDebug) << "[ListInfoElemNode], ParentDirectory: "<<file_info.parent_directory;
			//	RequestDispatcher::dir_info_st dir_info;
			//	if (getRequestDispatcher()->get_directory_info(userId,file_info.parent_directory,dir_info,status)){
					Log(Log::LogMsgDebug) << "[ListInfoElemNode], Dir owner: "<<file_info.owner;
					RequestDispatcher::user_info_st user_info;
					if (getRequestDispatcher()->get_user_info(file_info.owner,user_info,status)) {
						Log(Log::LogMsgDebug) << "[ListInfoElemNode], user last mod "<<file_info.user_last_mod;
						RequestDispatcher::user_info_st user_info_updated;
						if (getRequestDispatcher()->get_user_info(file_info.user_last_mod, user_info_updated, status)) {
							result=true;
							item
							<< "\"owner\":"
							<< "{\"firstName\":\"" << user_info.first_name << "\","
							<< "\"lastName\":\"" << user_info.last_name << "\","
							<< "\"email\":\"" << user_info.email << "\"},"

							<< "\"file\":"
							<< "{\"id\":\"" << Id << "\","
							<< "\"name\":\"" << file_info.name << "\","
							<< "\"size\":\"" << file_info.size << "\","
							<< "\"type\":\"" "a\","
							<< "\"shared\":\"" << "false" << "\"," //TODO : Ver de donde sacar el campo este
							<< "\"lastModDate\":\"" << file_info.date_last_mod << "\","
							<< "\"cantItems\":\"" "3\"},"


							<< "\"updatedBy\":"
							<< "{\"firstName\":\"" << user_info_updated.first_name << "\","
							<< "\"lastName\":\"" << user_info_updated.first_name << "\","
							<< "\"email\":\"" << user_info.email << "\"},"

							<< "\"collaborators\":";
							item << "[";

							vector<RequestDispatcher::user_info_st> lista_user_info;
							RequestDispatcher::file_info_st file_info;

							if (getRequestDispatcher()->get_file_info(userId,Id,file_info,status)) {
								lista_user_info=file_info.shared_users;
								Log(Log::LogMsgDebug) << "[ListOwnersNode]: list owners users " << lista_user_info.size();
								if (lista_user_info.size() != 0) {
									for (int i = 0; i < lista_user_info.size() - 1; ++i) {
										item
										<< "{\"id\":\"" << lista_user_info[i].id << "\","
										<< "\"firstName\":\"" << lista_user_info[i].first_name << "\","
										<< "\"lastName\":\"" << lista_user_info[i].last_name << "\","
										<< "\"email\":\"" << lista_user_info[i].email << "\"},";
										result = true;
									}
									item
									<< "{\"id\":\"" << lista_user_info[lista_user_info.size() - 1].id << "\","
									<< "\"firstName\":\"" << lista_user_info[lista_user_info.size() - 1].first_name << "\","
									<< "\"lastName\":\"" << lista_user_info[lista_user_info.size() - 1].last_name << "\","
									<< "\"email\":\"" << lista_user_info[lista_user_info.size() - 1].email << "\"}";
									result = true;
									Log(Log::LogMsgDebug) << "[ListOwnersNode] last node added ";
								} else {
									result = true;
								}
							}

							item << "]";

							vector<string> tags;
							item
							<< ",\"tags\":\"";
							if (getRequestDispatcher()->get_tags(userId,tags,status)){

								for (int i = 0; i < tags.size(); ++i) {
									item << tags[i]+";";
								}
								item << "\"";
							}

						}
			//		}
				}
			}

		}

		item << "}";

		if (!result) {
			Log(Log::LogMsgDebug) << "[ListInfoElemNode]: STATUS_CODE_NO_CONTENT";
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		} else {
			Log(Log::LogMsgDebug) << "[ListInfoElemNode]: STATUS_CODE_OK";
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);

			const std::string tmp = item.str();
			const char* msg = tmp.c_str();
			Log(Log::LogMsgDebug) << msg;
			getConnection().printfData(msg);
		}
	}
	else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string ListInfoElemNode::defaultResponse(){
	return "[]";
}

std::string ListInfoElemNode::getUserId(){
	vector<string> lista=ListInfoElemNode::split(getUri(),'/');
	return lista[3];
}

