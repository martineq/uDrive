/*
 * token_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef __TOKEN_NODE_H__
#define __TOKEN_NODE_H__

#include "../rest/node.h"
#include "../util/log.h"

class TokenNode : public Node {
	public:
		TokenNode();
		~TokenNode();

	protected:
		void executePost(MgConnectionW& conn, const char* url);
		std::string CreateToken(const std::string& email);
};

#endif
