/*
 * signup_node.cc
 *
 *  Created on: 4/10/2015
 *      Author: Martin
 */

#include "signup_node.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::vector;

SignupNode::SignupNode()  : Node("SignupNode") {
}

SignupNode::~SignupNode() {
}

void SignupNode::executePost(MgConnectionW& conn, const char* url){
		Log(Log::LogMsgDebug) << "[" << "SignUp " << "]";
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
		const Json::Value firstname = root["firstname"];
		const Json::Value lastname = root["lastname"];
		const Json::Value email = root["email"];
		const Json::Value password = root["password"];

		std::string firstnameS = firstname.asString();
		std::string lastnameS = lastname.asString();
		std::string emailS = email.asString();
		std::string passwordS = password.asString();

		Log(Log::LogMsgDebug) << "[" << "SignUp " << "] firstname: " << firstnameS << " Lastname: " << lastnameS << " Email: " << emailS << " Password: " << passwordS;

		int status=5;
		if ( (emailS != "") && (passwordS != "") ){ 
			int status;
			time_t now = time(0);
			char* dt = ctime(&now);
			std::string fecha(dt);
			std::string userId;
			std::string new_token=CreateToken(emailS);

			if (!this->rd->sign_up(emailS, passwordS, firstnameS, "Caba", new_token, fecha, userId,status)){
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


