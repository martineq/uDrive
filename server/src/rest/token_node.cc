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
	// Tomo usuario y pass de la conexion
	std::string email = conn.getVarStr("email");
	std::string password = conn.getVarStr("password");

	 Log(Log::LogMsgDebug) << "[" << "Validando usuario" << "] " << email << " " << password;

		if (!1){ //BD:Valido si el usuario existe en la base de datos
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"message\": \"user not found\", \"code\": %d, \"error_user_msg\": \"Usuario desconocido.\" }", MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			return;
		}

		if (!1){ //BD:Valido si el password es correcto en la BD.
			conn.sendStatus(MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"message\": \"wrong password\", \"code\": %d, \"error_user_msg\": \"Contraseña incorrecta.\" }", MgConnectionW::STATUS_CODE_UNAUTHORIZED);
			return;
			}

		if(1){ //Si el usuario y password es valido

			int userid=randomNumber(9999);//genero id usuario aleatorio solo para test.
			string token=CreateToken(email);
			conn.sendStatus(MgConnectionW::STATUS_CODE_CREATED);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			conn.printfData("{ \"userId\": \"%i\",  \"email\": \"%s\",  \"token\": \"%s\" }", userid, email.c_str(), token.c_str());
			return;
		}
		conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
		conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
		conn.printfData("{ \"message\": \"Bad Request\",  \"error_user_msg\": \"Mensaje Erroneo\"}");
}

std::string TokenNode::CreateToken(const std::string& email){
	stringstream ss;
	ss << randomNumber(1000) << email << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	return out;
}


