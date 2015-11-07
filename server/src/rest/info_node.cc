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

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		string userId=getUserId();
		dirId=lista[5];
		string token=getConnection().getAuthorization();
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;
		DataHandler::dir_info_st dirInfo;
		if (!getRequestDispatcher()->get_directory_info(userId, /*token,*/ dirId, dirInfo, status)){ //TODO(martindonofrio): use RequestDispatcher::check_token()
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
		else{
			vector<RequestDispatcher::info_element_st> directory_element_info;
			bool enc = false;
			std::ostringstream item;
  			item << "[";
			if (getRequestDispatcher()->get_directory_element_info_from_dir_info(userId, /*token,*/ dirInfo, directory_element_info, status)){  //TODO(martindonofrio): use RequestDispatcher::check_token()
				vector<RequestDispatcher::info_element_st>::iterator directory_it;
				Log(Log::LogMsgDebug) << "[" << "touring list" << "]: dirInfo: " << dirInfo.name;
				if (directory_element_info.size()!=0){
					for (directory_it = directory_element_info.begin(); directory_it < (directory_element_info.end()-1); directory_it++){
	     				enc=true;
	     					item 
		     				<< "{\"id\":\"" << (*directory_it).id 
		     				<< "\",\"name\":\"" << (*directory_it).name 
		     				<< "\",\"size\":\""	<< (*directory_it).size	
		     				<< "\",\"type\":\""	<< (*directory_it).type 
		     				<< "\",\"cantItems\":\"" << (*directory_it).number_of_items 
		     				<< "\",\"shared\":\"" << (*directory_it).shared 
		     				<< "\",\"lastModDate\":\"" << (*directory_it).lastModDate << "\"},";
					}
				}
				if (directory_element_info.size()==1) enc=true;
				if (!enc){
					//empty dir
					status=12;
					getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
					getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
					string msg=handlerError(status);
					getConnection().printfData(msg.c_str());
				}else{
					item
	     				<< "{\"id\":\"" << (*(directory_it)).id 
	     				<< "\",\"name\":\"" << (*(directory_it)).name 
	     				<< "\",\"size\":\""	<< (*(directory_it)).size	
	     				<< "\",\"type\":\""	<< (*(directory_it)).type 
	     				<< "\",\"cantItems\":\"" << (*(directory_it)).number_of_items 
	     				<< "\",\"shared\":\"" << (*(directory_it)).shared 
	     				<< "\",\"lastModDate\":\"" << (*(directory_it)).lastModDate << "\"}";
	     				item << "]";
					Log(Log::LogMsgDebug) << "[" << "listing directory" << "]: dirInfo: " << dirInfo.name << ", Number of items: " << directory_element_info.size();
					getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
					getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
					const std::string tmp = item.str();
					const char* msg = tmp.c_str();
					getConnection().printfData(msg);
				}

			} else Log(Log::LogMsgDebug) << "[" << "Not directory elem with dir_info" << "]";
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
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\"}]";
}

std::string InfoNode::getUserId(){
	vector<string> lista=InfoNode::split(getUri(),'/');
	return lista[3];
}

