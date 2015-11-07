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

SignupNode::SignupNode()  : Node("SignupNode") {
}

SignupNode::~SignupNode() {
}

void SignupNode::executePost(MgConnectionW& conn, const char* url){
		Log(Log::LogMsgDebug) << "[" << "SignUp " << "]";
		std::string firstname = conn.getBodyJson("firstname");
		std::string lastname = conn.getBodyJson("lastname");
		std::string email = conn.getBodyJson("email");
		std::string password = conn.getBodyJson("password");

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

			if (!this->rd->sign_up(email, password, firstname, lastname,"15.23","20.43", new_token, fecha, userId,status)){
				conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
				conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				string msg=handlerError(status);
				conn.printfData(msg.c_str());
			}else{
				conn.sendStatus(MgConnectionW::STATUS_CODE_OK);
				conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
				Log(Log::LogMsgDebug) << "[" << "SignUp - resultCode: 1 ]";
				conn.printfData("{\"resultCode\": 1}");
			}
		}else{
			Log(Log::LogMsgDebug) << "[" << "empty email or password" << "]";
			conn.sendStatus(MgConnectionW::STATUS_CODE_BAD_REQUEST);
			conn.sendContentType(MgConnectionW::CONTENT_TYPE_JSON);
			string msg=handlerError(status);
			conn.printfData(msg.c_str());
		}
}

std::string SignupNode::CreateToken(const std::string& email){
	stringstream ss;
	ss << randomNumber(1000) << email << time(NULL) << randomNumber(9999) ;
	string out;
	md5Str(out, ss.str());
	return out;
}

void SignupNode::setRequestDispatcher(RequestDispatcher* rd){
	this->rd=rd;
}
std::string SignupNode::defaultResponse(){
	return "{\"resultCode\": \"2\"}";
}


