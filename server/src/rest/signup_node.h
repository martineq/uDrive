/*
 * Signup_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef SIGNUP_NODE_H_
#define SIGNUP_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"
#include "../lib/json/json.h"
#include "../util/random_number.h"
#include "../util/md5.h"

class SignupNode  : public Node {

public:
	SignupNode();
	~SignupNode();
	void setRequestDispatcher(RequestDispatcher* rd);
protected:
	void executePost(MgConnectionW& conn, const char* url);
private:
	RequestDispatcher* rd;
	std::string CreateToken(const std::string& email);

};

#endif /* SIGNUP_NODE_H_ */
