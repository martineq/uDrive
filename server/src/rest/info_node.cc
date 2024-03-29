/*
 * info_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "info_node.h"

using std::string;
using std::stringstream;
using std::vector;

InfoNode::InfoNode(MgConnectionW&  conn)  : Node(conn) {
}

InfoNode::~InfoNode() {
}

vector<string> InfoNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void InfoNode::executeGet() {
	vector<string> lista=InfoNode::split(getUri(),'/');
	string dirId="";
	int status=11;
	Log(Log::LogMsgDebug) << "[InfoNode]";

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		string userId=getUserId();
		dirId=lista[5];

		Log(Log::LogMsgDebug) << "[InfoNode], UserId: " <<userId <<" dirId: "<<dirId;

        RequestDispatcher::dir_info_st dirInfo_rd;

		if (!getRequestDispatcher()->get_directory_info(userId, dirId, dirInfo_rd, status)){
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
			vector<RequestDispatcher::info_element_st> directory_element_info = dirInfo_rd.directory_element_info;   // Assign value
			bool enc = false;
			std::ostringstream item;
			string lastVersion="1";
  			item << "[";
			RequestDispatcher::file_info_st file_info;
			vector<RequestDispatcher::info_element_st>::iterator directory_it;
			Log(Log::LogMsgDebug) << "[" << "InfoNode" << "], touring list: dirInfo: " << dirInfo_rd.name;
			if (directory_element_info.size()!=0){
				RequestDispatcher::file_info_st file_info;
				for (directory_it = directory_element_info.begin(); directory_it < (directory_element_info.end()-1); directory_it++){
						if ((*directory_it).type=="a") {
							std::stringstream fileId;
							fileId << ((*directory_it).id);
							if (getRequestDispatcher()->get_file_info(userId,fileId.str(),file_info,status)) lastVersion=file_info.revision;
						}
						enc=true;
						item
						<< "{\"id\":\"" << (*directory_it).id << "\","
						<< "\"name\":\"" << (*directory_it).name << "\","
						<< "\"size\":\""	<< (*directory_it).size << "\","
						<< "\"type\":\""	<< (*directory_it).type << "\","
						<< "\"cantItems\":\"" << (*directory_it).number_of_items << "\","
					    << "\"shared\":\"" << (*directory_it).shared << "\", "
						<< "\"lastModDate\":\"" << (*directory_it).lastModDate << "\", "
						<< "\"userOwner\":\"" << (*directory_it).owner << "\", "
						<< "\"lastVersion\":\"" << lastVersion << "\"},";
				}
			}
			if (directory_element_info.size()==1) enc=true;
			if (!enc){
				//empty dir
				Log(Log::LogMsgDebug) << "[" << "InfoNode" << "], dirInfo: " << dirInfo_rd.name << ", -- Empty dir.";
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				getConnection().printfData(defaultResponse().c_str());
			}else{

				if ((*directory_it).type=="a") {
					std::stringstream fileId;
					fileId << ((*directory_it).id);
					if (getRequestDispatcher()->get_file_info(userId,fileId.str(),file_info,status)) lastVersion=file_info.revision;
				}

				item
				<< "{\"id\":\"" << (*directory_it).id << "\","
				<< "\"name\":\"" << (*directory_it).name << "\","
				<< "\"size\":\""	<< (*directory_it).size << "\","
				<< "\"type\":\""	<< (*directory_it).type << "\","
				<< "\"cantItems\":\"" << (*directory_it).number_of_items << "\","
				<< "\"shared\":\"" << (*directory_it).shared << "\", "
				<< "\"lastModDate\":\"" << (*directory_it).lastModDate << "\", "
				<< "\"userOwner\":\"" << (*directory_it).owner << "\", "
				<< "\"lastVersion\":\"" << lastVersion<< "\"}";
				item << "]";
				Log(Log::LogMsgDebug) << "[" << "InfoNode" << "], listing directory,  dirInfo: " << dirInfo_rd.name << ", -- Number of items: " << directory_element_info.size();
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				const std::string tmp = item.str();
				const char* msg = tmp.c_str();
				Log(Log::LogMsgDebug) << "[" << "InfoNode" << "]:" <<msg;
				getConnection().printfData(msg);
			}
		}
	}
	else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string InfoNode::defaultResponse(){
	return "[]";
}

std::string InfoNode::getUserId(){
	vector<string> lista=InfoNode::split(getUri(),'/');
	return lista[3];
}

