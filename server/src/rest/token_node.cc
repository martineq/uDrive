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

void TokenNode::execute(MgConnectionW& conn, const char* url){

	// Tomo usuario y pass de la conexion
	std::string email = conn.getVarStr("email");
	std::string password = conn.getVarStr("password");

	 Log(Log::LogMsgDebug) << "[" << "Validando usuario" << "] " << email << " " << password;


		//Valido si el usuario existe en la base de datos y si la contraseña es correcta
		if (!1){
			conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"userId\": \"0\",  \"email\": \"\",  \"token\": \"\" }");
			return;
		}

		if(1){
			//genero id usuario aleatorio solo para test.
			int userId=randomNumber(9999);
			string token=CreateToken(email);
			conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"userId\": \"%i\",  \"email\": \"%s\",  \"token\": \"%s\" }", userId, email.c_str(), token.c_str());
			return;
		}
		
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("{ \"userId\": \"0\",  \"email\": \"\",  \"token\": \"\" }");
}

std::string TokenNode::CreateToken(const std::string& email){
	stringstream ss;
	ss << randomNumber(1000) << email << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	return out;
}


