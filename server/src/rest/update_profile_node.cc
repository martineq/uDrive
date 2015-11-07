//
// Created by martin on 06/11/15.
//

#include "update_profile_node.h"

using std::string;
using std::stringstream;
using std::vector;

UpdateProfileNode::UpdateProfileNode(MgConnectionW&  conn)  : Node(conn) {
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

void UpdateProfileNode::executePut() {
    vector<string> lista=UpdateProfileNode::split(getConnection().getUri(),'/');
    int status=11;

    if (lista.size()==3){
        string userId=lista[2];
        string token=getConnection().getAuthorization();
        Log(Log::LogMsgDebug) << "[" << "Authorization " << "] token: " << token << " UserID: " << userId;
        DataHandler::user_info_st user_info;

        // Esto hay que cambiarlo por un metodo que me permite updatear los campos del usuario
        if (!getRequestDispatcher()->get_user_info(userId,/*token,*/user_info,status)){   //TODO(martindonofrio): use RequestDispatcher::check_token()
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg=handlerError(status);
            getConnection().printfData(msg.c_str());
        }
        else{
            std::ostringstream item;
            Log(Log::LogMsgDebug) << "[" << "updating photo profile" << "]: firstname: " << user_info.name;
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            Log(Log::LogMsgDebug) << "[" << "update profile - resultCode: 1 ]";
            getConnection().printfData("{\"resultCode\": 1}");
        }
    }else{
        getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
        getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
        string msg=handlerError(status);
        getConnection().printfData(msg.c_str());
    }
}
std::string UpdateProfileNode::defaultResponse(){
    return "{\"resultCode\": 2}";

}




