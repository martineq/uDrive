/*
 * info_trash_node.cc
 *
 *  Created on: 15/11/2015
 *      Author: Martin
 */

#include "info_trash_node.h"

using std::string;
using std::vector;

InfoTrashNode::InfoTrashNode(MgConnectionW&  conn)  : Node(conn) {
}

InfoTrashNode::~InfoTrashNode() {
}

vector<string> InfoTrashNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void InfoTrashNode::executeGet() {
	vector<string> lista=InfoTrashNode::split(getUri(),'/');
	string dirId="";
	int status=11;
	if ( (!lista[4].compare("trash")) && (lista.size()==5)){
		string userId=getUserId();
		Log(Log::LogMsgDebug) << "[InfoTrashNode], UserId: " <<userId <<" dirId: "<<dirId;

		vector<RequestDispatcher::info_element_st> deleted_files;

		if (!getRequestDispatcher()->get_deleted_files(userId,deleted_files,status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
			bool enc = false;
			std::ostringstream item;
  			item << "[";
			vector<RequestDispatcher::info_element_st>::iterator deleted_files_it;
			Log(Log::LogMsgDebug) << "[InfoTrashNode], touring list: deleted_files";
			if (deleted_files.size()!=0){
				for (deleted_files_it = deleted_files.begin(); deleted_files_it < (deleted_files.end()-1); deleted_files_it++){
					enc=true;
						item
						<< "{\"id\":\"" << (*deleted_files_it).id
						<< "\",\"name\":\"" << (*deleted_files_it).name
						<< "\",\"size\":\""	<< (*deleted_files_it).size
						<< "\",\"type\":\""	<< (*deleted_files_it).type
						<< "\",\"cantItems\":\"" << (*deleted_files_it).number_of_items
						<< "\",\"shared\":\"" << (*deleted_files_it).shared
						<< "\",\"lastModDate\":\"" << (*deleted_files_it).lastModDate
						<< "\",\"userOwner\":\"" << (*deleted_files_it).owner
						<< "\"},";
				}
			}
			if (deleted_files.size()==1) enc=true;
			if (!enc){
				//empty dir
				Log(Log::LogMsgDebug) << "[InfoTrashNode], userId: " << userId << ", -- Empty dir.";
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				getConnection().printfData(defaultResponse().c_str());
			}else{
				item
					<< "{\"id\":\"" << (*(deleted_files_it)).id
					<< "\",\"name\":\"" << (*(deleted_files_it)).name
					<< "\",\"size\":\""	<< (*(deleted_files_it)).size
					<< "\",\"type\":\""	<< (*(deleted_files_it)).type
					<< "\",\"cantItems\":\"" << (*(deleted_files_it)).number_of_items
					<< "\",\"shared\":\"" << (*(deleted_files_it)).shared
					<< "\",\"lastModDate\":\"" << (*(deleted_files_it)).lastModDate
				  	<< "\",\"userOwner\":\"" << (*deleted_files_it).owner
					<< "\"}";
					item << "]";
				Log(Log::LogMsgDebug) << "[" << "InfoTrashNode" << "], listing trash, userId: " << userId << ", --Number of items: " << deleted_files.size();
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				const std::string tmp = item.str();
				const char* msg = tmp.c_str();
				getConnection().printfData(msg);
				}
			}
	}
	else{
		Log(Log::LogMsgDebug) << "[InfoTrashNode], URL: "<<getUri();
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}

std::string InfoTrashNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\", \"userOwner \":\"0\"}]";
}

std::string InfoTrashNode::getUserId(){
	vector<string> lista=InfoTrashNode::split(getUri(),'/');
	return lista[3];
}

