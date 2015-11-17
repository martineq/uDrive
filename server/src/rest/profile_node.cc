//
// Created by martin on 06/11/15.
//

#include "profile_node.h"

using std::string;
using std::vector;

ProfileNode::ProfileNode(MgConnectionW&  conn)  : Node(conn) {
}

ProfileNode::~ProfileNode() {
}

void ProfileNode::executeGet() {
    vector<string> lista=ProfileNode::split(getConnection().getUri(),'/');
    int status=11;

    if (lista.size()==3){
        string userId=getUserId();
        Log(Log::LogMsgDebug) << "[ProfileNode] UserId: "<<userId;
        RequestDispatcher::user_info_st user_info;

        if (!getRequestDispatcher()->get_user_info(userId,user_info,status)){
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg=handlerError(status);
            getConnection().printfData(msg.c_str());
        }
        else{

            std::ostringstream item;
            std::string user_image;
            getRequestDispatcher()->get_user_image(userId,user_image,status);
            Log(Log::LogMsgDebug) << "[ProfileNode] TamañoDeImagen desde la base: "<<user_image.size();
            Log(Log::LogMsgDebug) << "[ProfileNode]: firstname: " << user_info.first_name;
            item
            << "{\"firstname\":\""  << user_info.first_name
            << "\",\"lastname\":\"" << user_info.last_name
            << "\",\"email\":\""	<< user_info.email;

            if (user_image.size()!=0)  item << "\",\"photo\":\"" << user_image.c_str();
            else item << "\",\"photo\":\"\"";

            item << "\",\"GPSLatitude\":\"" << user_info.gps_lat
            << "\",\"GPSLongitude\":\"" << user_info.gps_lon
            << "\",\"userId\":\"" << userId
            << "\",\"quotaUsed\":\"" << user_info.user_quota_used
            << "\",\"quotaTotal\":\"" << user_info.user_quota_total
            << "\",\"quotaUsagePercent\":\"" << user_info.user_quota_used_percentage
            << "\"}";

            getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            const std::string tmp = item.str();
            const char* msg = tmp.c_str();
            Log(Log::LogMsgDebug) << "[" << "ProfileNode" << "] item: " << msg;
            getConnection().printfData(msg);
            }
    }else{
        getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
        getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
        string msg=handlerError(status);
        getConnection().printfData(msg.c_str());
    }
}

std::string ProfileNode::defaultResponse(){
    return "[{ \"firstname\": \"0\",  \"lastname\": \"\","
            "\"email\": \"\" ,  \"photo\": \"\",  \"GPSLatitude\": \"0\", "
            "\"GPSLongitude\": \"0\",  \"userId\": \"0\""
            ",  \"quotaUsed\": \"0\",  \"quotaTotal\": \"0\",  \"quotaUsagePercent\": \"0\"}]";
}

vector<string> ProfileNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

std::string ProfileNode::getUserId() {
    vector<string> lista=ProfileNode::split(getUri(),'/');
    return lista[2];
}

