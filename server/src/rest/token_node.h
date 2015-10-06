#ifndef __TOKEN_NODE_H__
#define __TOKEN_NODE_H__

#include "../rest/node.h"
#include "../util/log.h"

class TokenNode : public Node {
	public:
		TokenNode();
		~TokenNode();

		void executePost(MgConnectionW& conn, const char* url);
	protected:
		std::string CreateToken(const std::string& email);
};

#endif
