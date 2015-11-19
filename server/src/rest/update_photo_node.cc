//
// Created by martin on 06/11/15.
//

#include "update_photo_node.h"

using std::string;
using std::stringstream;
using std::vector;

UpdatePhotoNode::UpdatePhotoNode(MgConnectionW&  conn)  : Node(conn) {
}

UpdatePhotoNode::~UpdatePhotoNode() {
}

vector<string> UpdatePhotoNode::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void UpdatePhotoNode::executePut() {
    vector<string> lista= UpdatePhotoNode::split(getConnection().getUri(),'/');
    int status=11;

    if (lista.size()==3){
        string userId=lista[2];
        Log(Log::LogMsgDebug) << "[UpdatePhotoNode]: userid: "<<userId;
        std::string photoStream="";
        photoStream=getConnection().getBodyJson("photoStream");

        string tam_string;
        stringstream mystream;

        mystream << photoStream.size();
        tam_string = mystream.str();

        Log(Log::LogMsgDebug) << "[UpdatePhotoNode]: Tome photoStream, tamaño "<<tam_string;

        if (!getRequestDispatcher()->set_user_image(userId,photoStream.c_str(),tam_string,status)){
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            string msg=handlerError(status);
            getConnection().printfData(msg.c_str());
        }
        else{
            Log(Log::LogMsgDebug) << "[" << "updating photo profile" << "]: UserID: " << userId;
            getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
            getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
            Log(Log::LogMsgDebug) << "[" << "update profile - resultCode: 1 ]";
            getConnection().printfData("{\"resultCode\": \"1\"}");
        }
    }else{
        getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
        getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
        string msg=handlerError(status);
        getConnection().printfData(msg.c_str());
    }
}

std::string UpdatePhotoNode::defaultResponse(){
    return "{\"resultCode\": \"2\"}";
}

std::string UpdatePhotoNode::getUserId() {
    vector<string> lista= UpdatePhotoNode::split(getUri(),'/');
    return lista[2];
}



