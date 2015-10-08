/*
 * info_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef INFO_NODE_H_
#define INFO_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>

class InfoNode  : public Node {

public:
	InfoNode();
	~InfoNode();
protected:
	void executeGet(MgConnectionW& conn, const char* url);
};

#endif /* INFO_NODE_H_ */
