/*
 * update_location_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "update_location_node.h"

using std::string;
using std::stringstream;
using std::vector;

UpdateLocationNode::UpdateLocationNode(MgConnectionW&  conn)  : Node(conn) {
}

UpdateLocationNode::~UpdateLocationNode() {
}

vector<string> UpdateLocationNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void UpdateLocationNode::executePut() {
	vector<string> lista = UpdateLocationNode::split(getUri(), '/');
	int status = 11;

	if (lista.size()==4) {
		Log(Log::LogMsgDebug) << "[UpdateLocationNode]";
		string userId = getUserId();

		Log(Log::LogMsgDebug) << "[UpdateLocationNode], UserId: " << userId;

		const Json::Value root = getConnection().getBodyJson();

		std::string GPSLatitude=root["GPSLatitude"].asString();
		std::string GPSLongitude=root["GPSLongitude"].asString();

		Log(Log::LogMsgDebug) << "[UpdateLocationNode]: GPSLatitude: " << GPSLatitude << ", GPSLongitude: " << GPSLongitude;

		RequestDispatcher::user_info_st user_info;
		getRequestDispatcher()->get_user_info(userId,user_info,status);

		if (!getRequestDispatcher()->modify_user_info(userId,user_info.email,user_info.first_name,user_info.last_name,GPSLatitude,GPSLongitude,status)) {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		} else {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			Log(Log::LogMsgDebug) << "[" << "UpdateLocationNode - resultCode: 1 ]";
			getConnection().printfData("{\"resultCode\": \"1\"}");
		}
	}else {
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg = handlerError(status);
			getConnection().printfData(msg.c_str());
		}
}
std::string UpdateLocationNode::defaultResponse(){
	return "{\"resultCode\": \"2\"}";
}

std::string UpdateLocationNode::getUserId(){
	vector<string> lista=UpdateLocationNode::split(getUri(),'/');
	return lista[3];
}

