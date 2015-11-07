/*
 * signup_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "signup_node.h"
using std::string;
using std::stringstream;
using std::vector;

SignupNode::SignupNode(MgConnectionW&  conn)  : Node(conn) {
}

SignupNode::~SignupNode() {
}

void SignupNode::executePost() {
		Log(Log::LogMsgDebug) << "[" << "SignUp " << "]";
		std::string firstname = getConnection().getBodyJson("firstname");
		std::string lastname = getConnection().getBodyJson("lastname");
		std::string email = getConnection().getBodyJson("email");
		std::string password = getConnection().getBodyJson("password");

		Log(Log::LogMsgDebug) << "[" << "SignUp " << "] firstname: " << firstname << ""
		" Lastname: " << lastname << " Email: " << email << " Password: " << password;

		int status=5;
		if ( (email != "") && (password != "") ){
			int status;
			time_t now = time(0);
			char* dt = ctime(&now);
			std::string fecha(dt);
			std::string userId;
			std::string new_token=CreateToken(email);

			if (!getRequestDispatcher()->sign_up(email, password, firstname, "Caba", new_token, fecha, userId,status)){
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				string msg=handlerError(status);
				getConnection().printfData(msg.c_str());
			}else{
				getConnection().sendStatus(MgConnectionW::STATUS_CODE_OK);
				getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				Log(Log::LogMsgDebug) << "[" << "SignUp - resultCode: 1 ]";
				getConnection().printfData("{\"resultCode\": 1}");
			}
		}else{
			Log(Log::LogMsgDebug) << "[" << "empty email or password" << "]";
			getConnection().sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			getConnection().sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			getConnection().printfData(msg.c_str());
		}
}

std::string SignupNode::CreateToken(const std::string& email){
	stringstream ss;
	ss << randomNumber(1000) << email << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	return out;
}

std::string SignupNode::defaultResponse(){
	return "{\"resultCode\": \"2\"}";
}

bool SignupNode::auth(int &status){
	Log(Log::LogMsgDebug) << "[" << "SignUp - always auth" << "]";
	return true;
}


