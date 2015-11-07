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
#include "../request_dispatcher.h"

class TokenNode : public Node {
	public:
		TokenNode(MgConnectionW& conn);
		~TokenNode();
	protected:
		void executePost();
		std::string CreateToken(const std::string& email);
		std::string defaultResponse();
		bool auth(int &status);
		
};

#endif
