#include "token_node.h"

#include <string>
#include <ctime>
#include <sstream>

#include "util/random_number.h"
#include "util/md5.h"

using std::string;
using std::stringstream;

TokenNode::TokenNode() : Node("token") {
}

void TokenNode::executeGet(MgConnection& conn, const char* url){
	// Tomo usuario y pass de la coneccion
	string email = conn.getVarStr("email");
	string password = conn.getVarStr("password");

	//Validar si es password es valido
		if(1){
			std::string token=TokenNode::CreateToken(email);
			conn.sendStatus(MgConnection::STATUS_CODE_CREATED);
			conn.sendContentType(MgConnection::CONTENT_TYPE_JSON);
			conn.printfData("{ \"userid\": \"%s\" , \"email\": \"%s\" , \"token\": \"%s\" }", "0", email, token.c_str());
			return;
		}
		conn.sendStatus(MgConnection::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnection::CONTENT_TYPE_JSON);
		conn.printfData("{ \"message\": \"%s\",  \"error_user_msg\": \"Problemas con el logeo\"}", email.c_str());
}

std::string CreateToken(const string& username){
	stringstream ss;
	ss << randomNumber(1000) << username << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	return out;
}
