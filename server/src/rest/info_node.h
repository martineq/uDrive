/*
 * info_node.h
 *
 *  Created on: 4/10/2015
 *      Author: root
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
	void execute(MgConnectionW& conn, const char* url);
};

#endif /* INFO_NODE_H_ */
