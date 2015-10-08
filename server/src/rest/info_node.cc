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

		if (token.compare("be16e465de64f0d2f2d83f3cfcd6370b")!=0){
			Log(Log::LogMsgDebug) << "[" << "invalid token" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
														"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
														"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
		/*}else if (dirId.compare("1000")!=0){
			Log(Log::LogMsgDebug) << "[" << "invalid dirId" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_NOT_FOUND);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
														"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
														"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
		}else if (userId.compare("1111")!=0){
			Log(Log::LogMsgDebug) << "[" << "invalid userId" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_NOT_FOUND);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
														"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
														"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");
		*/}else{
			Log(Log::LogMsgDebug) << "[" << "retrieve list" << "]";

			const std::string dirs[4] = {"Photos", "Music", "Documents", "Movies"};
			const std::string files[4] = {"Doc.pdf", "My stylesheet.xls", "PIC0001.jpg", "U2-One.mp3"};
			const std::string dates[4] = {"01/05/2006", "10/05/2014", "13/09/2015", "18/11/2014"};
			
			srand ( time(NULL) ); //initialize the random seed
  			int randIndex = rand() % 4; //generates a random number between 0 and 3
  			std::ostringstream item;
  			item << "[";
  			// load some folders
  			std::string shared = "true";
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
			/*conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\",  "
												"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
												"\"shared\": \"%s\",  \"lastModDate\": \"%s\"},"
							  "{\"id\": \"%d\",  \"name\": \"%s\",  "
												"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
												"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]"
					, 1000, "Carpeta1", 1024,"d",2,"true","10/08/2015", 1001, "Archivo1", 4096,"a",3,"false","20/12/2015");*/
			const std::string tmp = item.str();
			const char* msg = tmp.c_str();
			conn.printfData(msg);
		}
	}else{
		Log(Log::LogMsgDebug) << "[" << "invalid url" << "]";
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("[{ \"id\": \"%d\",  \"name\": \"%s\","
											"\"size\": \"%d\" ,  \"type\": \"%s\",  \"cantItems\": \"%d\", "
											"\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]", 0, "", 0,"",0,"","");

	}

}

