/*
 * info_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "info_node.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::vector;

InfoNode::InfoNode()  : Node("Info") {
}

InfoNode::~InfoNode() {
}

vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

/**
 *
 */
void InfoNode::executeGet(MgConnectionW& conn, const char* url){
	vector<string> lista=split(url,'/');
	string dirId="";
	int status=11;

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		string userId=lista[3];
		dirId=lista[5];
		string token=conn.getAuthorization();
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;
		DataHandler::dir_info_st dirInfo;
		if (!this->rd->get_directory_info(userId, token, dirId, dirInfo, status)){
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			conn.printfData(msg.c_str());
		}
		else{
			vector<RequestDispatcher::info_element_st> directory_element_info;
			bool enc = false;
			std::ostringstream item;
  			item << "[";
			if (this->rd->get_directory_element_info_from_dir_info(userId, token, dirInfo, directory_element_info, status)){
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
					conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
					conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
					string msg=handlerError(status);
					conn.printfData(msg.c_str());
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
  					conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
					conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
					const std::string tmp = item.str();
					const char* msg = tmp.c_str();
					conn.printfData(msg);

				}

			} else Log(Log::LogMsgDebug) << "[" << "Not directory elem with dir_info" << "]";
			}	
	}
	else{
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		conn.printfData(msg.c_str());
	}
}

void InfoNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}

std::string InfoNode::defaultResponse(){
	return "[{ \"id\": \"0\",  \"name\": \"\","
							"\"size\": \"0\" ,  \"type\": \"\",  \"cantItems\": \"0\", "
							"\"shared\": \"\",  \"lastModDate\": \"\"}]";

}

