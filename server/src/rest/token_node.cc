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

TokenNode::TokenNode(MgConnectionW&  conn)  : Node(conn) {
}

void TokenNode::executePost() {
	Log(Log::LogMsgDebug) << "[" << "TokenNode" << "]: parsing Json";

	std::string email = getConnection().getBodyJson("email");
	std::string password = getConnection().getBodyJson("password");

	Log(Log::LogMsgDebug) << "[" << "validating user" << "] " << email << " " << password;

	string new_token=CreateToken(email);
	string userId="";
	int status;

	if (!getRequestDispatcher()->log_in(email, password, new_token, userId, status)){
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		string msg=handlerError(status);
		getConnection().printfData(msg.c_str());
	}else{
		Log(Log::LogMsgDebug) << "[" << "Valid user!!: userId: " << userId << "] " << "Token: " <<new_token.c_str();
		getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
		getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		getConnection().printfData("{ \"userId\": \"%s\",  \"email\": \"%s\",  \"token\": \"%s\", \"quotaAvailable\": \"%s\" }", userId.c_str(), email.c_str(), new_token.c_str(),"0");
	}
}
string TokenNode::CreateToken(const std::string& email){
	stringstream ss;
	ss << randomNumber(1000) << email << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	return out;
}

std::string TokenNode::defaultResponse(){
	return "{ \"userId\": \"0\",  \"email\": \"\",  \"token\": \"\", \"quotaAvailable\": \"0\"}";
}

bool TokenNode::auth(int &status){
	Log(Log::LogMsgDebug) << "Auth de Token";
	std::string email = getConnection().getBodyJson("email");
	std::string password = getConnection().getBodyJson("password");
	Log(Log::LogMsgDebug) << "[" << "auth-TOKEN" << "] Email: " << email << ", Password: "  << password;
	return true;
}


