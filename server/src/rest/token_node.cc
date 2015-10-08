#include "token_node.h"
#include "../lib/json/json.h"

#include <ctime>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

using std::vector;

#include "../util/random_number.h"
#include "../util/md5.h"

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

	const char *s = conn->content;
	char body[1024*sizeof(char)] = "";

	strncpy(body, s, conn->content_len);
	body[conn->content_len] = '0';

	// Parse the JSON body
	Json::Value root;
	Json::Reader reader;
	bool parsedSuccess = reader.parse(body, root, false);
	if (!parsedSuccess) {
		// Error, do something
	}
	const Json::Value mail = root["email"];
	const Json::Value pass = root["password"];

	std::string email = mail.asString();
	std::string password = pass.asString();

	 Log(Log::LogMsgDebug) << "[" << "Validando usuario" << "] " << email << " " << password;

		if (email.compare("mail@mail.com")!=0){ //BD:Valido si el mail existe en la base de datos
			Log(Log::LogMsgDebug) << "[" << "email incorrecto" << "] ";
			conn.sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"userId\": \"%d\",  \"email\": \"%s\",  \"token\": \"%s\" }", 0, "", "");
		}
		else if (password.compare("90d0c318f2c19b8f5477a9b52d5e6b63")!=0){ //BD:Valido si el password es correcto en la BD.
			Log(Log::LogMsgDebug) << "[" << "password incorrecto" << "] ";
			conn.sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData( "{ \"userId\": \"%d\",  \"email\": \"%s\",  \"token\": \"%s\" }", 0, "", "");
			return;
		}
		else{
			Log(Log::LogMsgDebug) << "[" << "Genero Token" << "] ";
				int userId=randomNumber(9999);
				string token=CreateToken(email);
				conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
				conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				conn.printfData("{ \"userId\": \"%d\",  \"email\": \"%s\",  \"token\": \"%s\" }", userId, email.c_str(), token.c_str());
				return;
		}
}


std::string TokenNode::CreateToken(const std::string& email){
	stringstream ss;
	ss << randomNumber(1000) << email << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	//return out;
	return "be16e465de64f0d2f2d83f3cfcd6370b";
}


