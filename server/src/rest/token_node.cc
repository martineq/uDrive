#include "token_node.h"
#include "../lib/json/json.h"

#include <ctime>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "../util/random_number.h"
#include "../util/md5.h"

using std::vector;
using std::string;
using std::stringstream;

struct email_pass{
	string email="";
	string pass="";
};

typedef struct email_pass email_pass;

TokenNode::TokenNode() : Node("token") {
}

void TokenNode::executePost(MgConnectionW& conn, const char* url){
	Log(Log::LogMsgDebug) << "[" << "TokenNode" << "]: parsing Json";

	std::string email = conn.getBodyJson("email");
	std::string password = conn.getBodyJson("password");

	Log(Log::LogMsgDebug) << "[" << "validating user" << "] " << email << " " << password;

	string new_token=CreateToken(email);
	string userId="";
	int status;

	if (!this->rd->log_in(email, password, new_token, userId, status)){
		conn.sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		conn.printfData(msg.c_str());
	}else{
		Log(Log::LogMsgDebug) << "[" << "Valid user: userId: " << userId << "] " << "Token: " <<new_token.c_str();
		conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("{ \"userId\": \"%s\",  \"email\": \"%s\",  \"token\": \"%s\" }", userId.c_str(), email.c_str(), new_token.c_str());
	}
}
string TokenNode::CreateToken(const std::string& email){
	stringstream ss;
	ss << randomNumber(1000) << email << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	return out;
}

void TokenNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}

std::string TokenNode::defaultResponse(){
return "{ \"userId\": \"0\",  \"email\": \"\",  \"token\": \"\" }";
}


