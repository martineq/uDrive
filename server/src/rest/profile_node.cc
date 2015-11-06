//
// Created by martin on 06/11/15.
//

#include "profile_node.h"

using std::string;
using std::stringstream;
using std::vector;

ProfileNode::ProfileNode()  : Node("Profile") {
}

ProfileNode::~ProfileNode() {
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

void ProfileNode::executeGet(MgConnectionW& conn, const char* url){
    vector<string> lista=ProfileNode::split(url,'/');
    int status=11;

    if (lista.size()==3){
        string userId=lista[2];
        string token=conn.getAuthorization();
        Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;
        DataHandler::user_info_st user_info;
        if (!this->rd->get_user_info(userId,/*token,*/user_info,status)){  //TODO(martindonofrio): use RequestDispatcher::check_token()
            conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
            conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg=handlerError(status);
            conn.printfData(msg.c_str());
        }
        else{
            std::ostringstream item;
            Log(Log::LogMsgDebug) << "[" << "printing profile" << "]: firstname: " << user_info.name;
            item
            << "{\"firstname\":\""  << user_info.name
            << "\",\"lastname\":\"" << "-"
            << "\",\"email\":\""	<< user_info.email
            << "\",\"photo\":\""	<< ""
            << "\",\"GPSLatitude\":\"" "-24.565131"
            << "\",\"GPSLongitude\":\"" << "-52.565351"
            << "\",\"userId\":\"" << userId
            << "\",\"quotaAvailable\":\"" << "570 MB"
            << "\",\"quotaTotal\":\"" << "756 MB"
            << "\",\"quotaUsagePercent\":\"" << "75.39%"
            << "\"}";
            conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
            conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            const std::string tmp = item.str();
            const char* msg = tmp.c_str();
            conn.printfData(msg);
        }
    }else{
        conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
        conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
        string msg=handlerError(status);
        conn.printfData(msg.c_str());
    }
}

void ProfileNode::setRequestDispatcher(RequestDispatcher* rd){
    this->rd=rd;
}

std::string ProfileNode::defaultResponse(){
    return "[{ \"firstname\": \"0\",  \"lastname\": \"\","
            "\"email\": \"\" ,  \"photo\": \"\",  \"GPSLatitude\": \"0\", "
            "\"GPSLongitude\": \"0\",  \"userId\": \"0\""
            ",  \"quotaAvailable\": \"0\",  \"quotaTotal\": \"0\",  \"quotaUsagePercent\": \"0\"}]";

}

