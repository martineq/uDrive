/*
 * create_dir_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "create_dir_node.h"
using std::string;
using std::stringstream;
using std::vector;

CreateDirNode::CreateDirNode(MgConnectionW&  conn)  : Node(conn) {
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

void CreateDirNode::executePost() {
	vector<string> lista=CreateDirNode::split(getUri(),'/');
	int status=11;
	
	if ( (!lista[3].compare("dir")) && (lista.size()==5)){
		string userId=lista[2];
		string dirId=lista[4];
		string token=getConnection().getAuthorization();
		Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;

		//TODO(martindonofrio): Validar que no sea vacio el campo dirName
		std::string dirNameS = getConnection().getBodyJson("dirName");
		std::string new_dirId;


		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];
		time (&rawtime);
		timeinfo = localtime (&rawtime);
		strftime (buffer,80," %d/%m/%Y %X",timeinfo);
		Log(Log::LogMsgDebug) << "[CreateDirNode]: fecha: "<<buffer;
		std::string fecha(buffer);

		Log(Log::LogMsgDebug) << "[" << "CreateDirNode " << "] userId: " << userId << " dirId: " << dirId << " Create directory " << dirNameS;
		if (getRequestDispatcher()->new_directory(userId, dirNameS, fecha, dirId, new_dirId, status)){

			RequestDispatcher::dir_info_st dirInfo_rd;
			if (!getRequestDispatcher()->get_directory_info(userId, dirId, dirInfo_rd, status)){
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				string msg=handlerError(status);
				getConnection().printfData(msg.c_str());
			}
			else{
				vector<RequestDispatcher::info_element_st> directory_element_info = dirInfo_rd.directory_element_info;  // Assign value
				bool enc = false;
				std::ostringstream item;
				string lastVersion="";
	  			item << "[";
				if ( true ){  //TODO(martindonofrio): delete "if" (not needed anymore)
					vector<RequestDispatcher::info_element_st>::iterator directory_it;
					Log(Log::LogMsgDebug) << "[" << "touring list" << "]: dirInfo: " << dirInfo_rd.name;
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
							<< "\",\"lastModDate\":\"" << (*directory_it).lastModDate
							<< "\",\"userOwner\":\"" << (*directory_it).owner
							<< "\",\"lastVersion\":\"" << lastVersion << "\"},";
						}
					}
					if (directory_element_info.size()==1) enc=true;
					
					if (!enc){
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
						<< "\",\"lastModDate\":\"" << (*directory_it).lastModDate
						<< "\",\"userOwner\":\"" << (*directory_it).owner
						<< "\",\"lastVersion\":\"" << lastVersion << "\"}";
	     				item << "]";

						Log(Log::LogMsgDebug) << "[" << "listing directory" << "]: dirInfo: " << dirInfo_rd.name << ", Number of items: " << directory_element_info.size();
	  					getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
						getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
						const std::string tmp = item.str();
						const char* msg = tmp.c_str();
						Log(Log::LogMsgDebug) << "[" << "listing directory" << "]:  " <<msg;
						getConnection().printfData(msg);
					}
				} else Log(Log::LogMsgDebug) << "[" << "Not directory elem with dir_info" << "]";
				}	
		}else{
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
	}else{
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
	}
}
std::string CreateDirNode::defaultResponse(){
	return "[{ \"id\": \"0\",\"name\": \"\",\"size\": \"0\","
				"\"type\": \"\",\"cantItems\": \"0\",\"shared\": \"\",\"lastModDate\": \"\",\"userOwner\": \"\",\"lastVersion\": \"\"}]";
}

std::string CreateDirNode::getUserId() {
	vector<string> lista=CreateDirNode::split(getUri(),'/');
	return lista[2];
}


