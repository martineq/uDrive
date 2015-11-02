/*
 * create_dir_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "create_dir_node.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::vector;

CreateDirNode::CreateDirNode()  : Node("CreateDir") {
}

CreateDirNode::~CreateDirNode() {
}

vector<string> CreateDirNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void CreateDirNode::executePost(MgConnectionW& conn, const char* url){
	vector<string> lista=CreateDirNode::split(conn->uri,'/');

	int status;
	
	if ( (!lista[3].compare("dir")) && (lista.size()==5)){
		string userId=lista[2];
		string dirId=lista[4];
		string token=conn.getAuthorization();
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;

		const char *s = conn->content;
		char body[1024*sizeof(char)] = "";
		strncpy(body, s, conn->content_len);
		body[conn->content_len] = '0';

		// Parse the JSON body
		Json::Value root;
		Json::Reader reader;
		bool parsedSuccess = reader.parse(body, root, false);
		if (!parsedSuccess) {
			// Error, do something
		}
		const Json::Value dirName = root["dirName"];
		std::string dirNameS = dirName.asString();
		std::string new_dirId;
		

		time_t now = time(0);
		char* dt = ctime(&now);
		std::string fecha(dt);

		Log(Log::LogMsgDebug) << "[" << "CreateDirNode " << "] userId: " << userId << " dirId: " << dirId << " Create directory " << dirNameS;
		if (this->rd->new_directory(userId, token, dirNameS, fecha, dirId, new_dirId, status)){
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
						Log(Log::LogMsgDebug) << "[" << "empty directory" << "]: dirInfo: " << dirInfo.name;
						conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
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

		//	conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
		//	conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		//	Log(Log::LogMsgDebug) << "[" << "CreateDirNode " << "] new dirId: " << new_dirId.c_str();
		//	conn.printfData("{\"dirId\": \"%s\"}",new_dirId.c_str());


		}else{
			Log(Log::LogMsgDebug) << "[" << "failed to create" << "] Status: " << status;
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			//conn.printfData("{\"dirId\": \"%s\"}","0");
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
											"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
											"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
		}
	}else{
			Log(Log::LogMsgDebug) << "failed to create" << "] Status: " <<status;
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			//conn.printfData("{\"dirId\": \"%s\"}","0");
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
											"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
											"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
	}
}

void CreateDirNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}


