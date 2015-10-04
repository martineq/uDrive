#ifndef __AUTH_NODE_H__
#define __AUTH_NODE_H__

#include "../rest/node.h"

class TokenNode : public Node {
	public:
		TokenNode();

	protected:
		void executeGet(MgConnection& conn, const char* url);
		static std::string CreateToken(const std::string& email);
};

#endif
