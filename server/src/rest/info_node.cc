/*
 * info_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: root
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

	if ( (!lista[4].compare("dir")) && (lista.size()==6)){
		string userId=lista[3];
		string dirId=lista[5];
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << conn.getAuthorization() << " UserID: " << userId;

		int status;
		string token=conn.getAuthorization();
		Log(Log::LogMsgDebug) << "[" << "retrieve list" << "]";
		
			Log(Log::LogMsgDebug) << "[" << "invalid token" << "]";

			DataHandler::dir_info_st dirInfo;
			if (!this->rd->get_directory_info(userId, token, dirId, dirInfo, status)){
				conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
				conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
														"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
														"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
			}
			else{

				vector< RequestDispatcher::info_element_st > vector_element_info;
				if (this->rd->get_directory_element_info_from_dir_info(userId, token, dirInfo, vector_element_info, status)){
					vector< RequestDispatcher::info_element_st >::iterator directory_it;

					for (directory_it=vector_element_info.begin(); directory_it != vector_element_info.end(); directory_it++){
         				cout << (*directory_it).id << endl;        
					}
 /*
					for(int k=10; k<=11; k++){
		  				item << "{\"id\":\"" << k << "\",\"name\":\"" << dirs[randIndex] << "\",\"size\":\"200\",\"type\":\"d\",\"cantItems\":\"5\",\"shared\":\"" << shared << "\",\"lastModDate\":\"" <<  dates[randIndex] << "\"}";
		  				if (k!=11)
		  					item << ",";
		  				shared = "false";
		  				randIndex = rand() % 4;
		  			}
		  			item << ",";
		  			// load some files
		  			for(int k=12; k<=13; k++){
		  				item << "{\"id\":\"" << k << "\",\"name\":\"" << files[randIndex] << "\",\"size\":\"200\",\"type\":\"a\",\"cantItems\":\"5\",\"shared\":\"" << shared << "\",\"lastModDate\":\"" <<  dates[randIndex] << "\"}";
		  				if (k!=13)
		  					item << ",";
		  				randIndex = rand() % 4;
		  			}
		  			item << "]";
					Log(Log::LogMsgDebug) << item.str();	

					conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
					conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
					const std::string tmp = item.str();
					const char* msg = tmp.c_str();
					conn.printfData(msg);

					*/
			}
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


