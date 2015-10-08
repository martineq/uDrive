#include "token_node.h"

#include <ctime>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>


#include "../util/random_number.h"
#include "../util/md5.h"

using std::string;
using std::stringstream;

TokenNode::TokenNode() : Node("token") {
}

void TokenNode::executePost(MgConnectionW& conn, const char* url){
	std::string email = conn.getVarStr("email");
	std::string password = conn.getVarStr("password");

	 Log(Log::LogMsgDebug) << "[" << "Validando usuario" << "] " << email << " " << password;

		if (email.compare("mail@mail.com")!=0){ //BD:Valido si el mail existe en la base de datos
			Log(Log::LogMsgDebug) << "[" << "email incorrecto" << "] ";
			conn.sendStatus(MgConnectionW::STATUS_CODE_NO_CONTENT);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"userId\": \"%d\",  \"email\": \"%s\",  \"token\": \"%s\" }", 0, "", "");
		}
		else if (password.compare("1234")!=0){ //BD:Valido si el password es correcto en la BD.
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
	return out;
}


