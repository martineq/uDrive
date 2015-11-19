//
// Created by martin on 06/11/15.
//

#include "update_userfullname_node.h"

using std::string;
using std::stringstream;
using std::vector;

UpdateUserFullNameNode::UpdateUserFullNameNode(MgConnectionW&  conn)  : Node(conn) {
}

UpdateUserFullNameNode::~UpdateUserFullNameNode() {
}

vector<string> UpdateUserFullNameNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void UpdateUserFullNameNode::executePut() {
    vector<string> lista= UpdateUserFullNameNode::split(getConnection().getUri(),'/');
    int status=11;

    if (lista.size()==3){
        string userId=lista[2];
        Log(Log::LogMsgDebug) << "[UpdateUserFullNameNode]: userid: "<<userId;

        std::string firstname=getConnection().getBodyJson("firstname");
        Log(Log::LogMsgDebug) << "[UpdateUserFullNameNode]: field firstname ok";

        std::string lastname=getConnection().getBodyJson("lastname");
        Log(Log::LogMsgDebug) << "[UpdateUserFullNameNode]: field lastname ok";
        bool enc=true;

        RequestDispatcher::user_info_st user_info;
        if (!getRequestDispatcher()->get_user_info(userId,user_info,status)){
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg=handlerError(status);
            getConnection().printfData(msg.c_str());
            enc=false;
        }else{
            Log(Log::LogMsgDebug) << "[UpdateUserFullNameNode]: retrieve user data ok";
        }

        if (!getRequestDispatcher()->modify_user_info(userId,user_info.email,firstname,lastname,user_info.gps_lat,user_info.gps_lat,status)) {
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg=handlerError(status);
            getConnection().printfData(msg.c_str());
        }
        else{
            Log(Log::LogMsgDebug) << "[UpdateUserFullNameNode] Updating info, UserID: " << userId;
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg="{\"resultCode\": \"1\"}";
            Log(Log::LogMsgDebug) << "[UpdateUserFullNameNode]: "<<msg;
            getConnection().printfData(msg.c_str());
        }
    }else{
        getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
        getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
        string msg=handlerError(status);
        getConnection().printfData(msg.c_str());
    }
}

std::string UpdateUserFullNameNode::defaultResponse(){
    return "{\"resultCode\": \"2\"}";
}

std::string UpdateUserFullNameNode::getUserId() {
    vector<string> lista= UpdateUserFullNameNode::split(getUri(),'/');
    return lista[2];
}



