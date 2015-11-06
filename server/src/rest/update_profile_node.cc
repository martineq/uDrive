//
// Created by martin on 06/11/15.
//

#include "update_profile_node.h"

using std::string;
using std::stringstream;
using std::vector;

UpdateProfileNode::UpdateProfileNode()  : Node("UpdateProfile") {
}

UpdateProfileNode::~UpdateProfileNode() {
}

vector<string> UpdateProfileNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void UpdateProfileNode::executePut(MgConnectionW& conn, const char* url){
    vector<string> lista=UpdateProfileNode::split(url,'/');
    int status=11;

    if (lista.size()==3){
        string userId=lista[2];
        string token=conn.getAuthorization();
        Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;
        DataHandler::user_info_st user_info;

        // Esto hay que cambiarlo por un metodo que me permite updatear los campos del usuario
        if (!this->rd->get_user_info(userId,token,user_info,status)){
            conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
            conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg=handlerError(status);
            conn.printfData(msg.c_str());
        }
        else{
            std::ostringstream item;
            Log(Log::LogMsgDebug) << "[" << "updating photo profile" << "]: firstname: " << user_info.name;
            conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
            conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            Log(Log::LogMsgDebug) << "[" << "update profile - resultCode: 1 ]";
            conn.printfData("{\"resultCode\": 1}");
        }
    }else{
        conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
        conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
        string msg=handlerError(status);
        conn.printfData(msg.c_str());
    }
}

void UpdateProfileNode::setRequestDispatcher(RequestDispatcher* rd){
    this->rd=rd;
}

std::string UpdateProfileNode::defaultResponse(){
    return "{\"resultCode\": 2}";

}

