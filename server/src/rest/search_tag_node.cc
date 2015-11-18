/*
 * search_tag_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "search_tag_node.h"

using std::string;
using std::stringstream;
using std::vector;

SearchTagNode::SearchTagNode(MgConnectionW&  conn)  : Node(conn) {
}

SearchTagNode::~SearchTagNode() {
}

vector<string> SearchTagNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void SearchTagNode::executeGet() {
	vector<string> lista=SearchTagNode::split(getUri(),'/');
	int status=11;
	if ( (!lista[4].compare("tags"))){
		Log(Log::LogMsgDebug) << "[SearchTagNode]";
		string userId=getUserId();
		bool result=false;

		string tag = getConnection().getParameter("tagname");
		Log(Log::LogMsgDebug) << "[SearchTagNode], UserId: " <<userId<< ", tag: " << tag;

		std::ostringstream item;
		vector<RequestDispatcher::info_element_st> elem_info;
		string lastVersion="1";

		if (getRequestDispatcher()->search_by_tag(userId,tag, elem_info,status)){
			item << "[";
			Log(Log::LogMsgDebug) << "[SearchTagNode]: listing element "<< elem_info.size();
			if (elem_info.size()!=0) {
				for (int i = 0; i < elem_info.size()-1 ; ++i) {
					item
					<< "{\"id\":\"" << (elem_info[i]).id << "\","
					<< "\"name\":\"" << (elem_info[i]).name << "\","
					<< "\"size\":\""	<< (elem_info[i]).size << "\","
					<< "\"type\":\""	<< (elem_info[i]).type << "\","
					<< "\"cantItems\":\"" << (elem_info[i]).number_of_items << "\","
					<< "\"shared\":\"" << (elem_info[i]).shared << "\", "
					<< "\"lastModDate\":\"" << (elem_info[i]).lastModDate << "\", "
					<< "\"userOwner\":\"" << (elem_info[i]).owner << "\", "
					<< "\"lastVersion\":\"" << lastVersion<< "\"},";
					result=true;
				}
				Log(Log::LogMsgDebug) << "[SearchTagNode]: "<< elem_info.size();
				int i=elem_info.size()-1;
				item
				<< "{\"id\":\"" << (elem_info[i]).id << "\","
				<< "\"name\":\"" << (elem_info[i]).name << "\","
				<< "\"size\":\""	<< (elem_info[i]).size << "\","
				<< "\"type\":\""	<< (elem_info[i]).type << "\","
				<< "\"cantItems\":\"" << (elem_info[i]).number_of_items << "\","
				<< "\"shared\":\"" << (elem_info[i]).shared << "\", "
				<< "\"lastModDate\":\"" << (elem_info[i]).lastModDate << "\", "
				<< "\"userOwner\":\"" << (elem_info[i]).owner << "\", "
				<< "\"lastVersion\":\"" << lastVersion<< "\"}";
				result=true;
				Log(Log::LogMsgDebug) << "[SearchTagNode]: ";
				item << "]";
			}else{
				item << "]";
				result = true;
			}

		}

		if (!result) {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		} else {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);

			const std::string tmp = item.str();
			const char* msg = tmp.c_str();
			Log(Log::LogMsgDebug) << tmp.c_str();
			getConnection().printfData(tmp.c_str());
		}
	}
	else{
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}
}
std::string SearchTagNode::defaultResponse(){
	return "[]";
}

std::string SearchTagNode::getUserId(){
	vector<string> lista=SearchTagNode::split(getUri(),'/');
	return lista[3];
}

