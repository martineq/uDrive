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

void InfoNode::executeGet(MgConnectionW& conn, const char* url){
	vector<string> lista=split(conn->uri,'/');
	string dirId="";

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		string userId=lista[3];
		dirId=lista[5];
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << conn.getAuthorization() << " UserID: " << userId;

		int status;
		string token=conn.getAuthorization();
		DataHandler::dir_info_st dirInfo;
		if (!this->rd->get_directory_info(userId, token, dirId, dirInfo, status)){
			Log(Log::LogMsgDebug) << "[" << "Fail get get_directory_info" << "]" << "Status: " << status;
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
													"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
													"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
		}
		else{
			vector<RequestDispatcher::info_element_st> directory_element_info;
			bool enc = false;
			std::ostringstream item;
  			item << "[";
			if (this->rd->get_directory_element_info_from_dir_info(userId, token, dirInfo, directory_element_info, status)){
				vector<RequestDispatcher::info_element_st>::iterator directory_it;
				Log(Log::LogMsgDebug) << "[" << "touring list" << "]: dirInfo: " << dirInfo.name;

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

				if (!enc){
					Log(Log::LogMsgDebug) << "[" << "empty directory" << "]: dirInfo: " << dirInfo.name;
					conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
					conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
					conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
										"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
										"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
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
		Log(Log::LogMsgDebug) << "[" << "invalid url" << "]";
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
										"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
										"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");

	}
}

void InfoNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}


