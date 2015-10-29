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
 #include "../request_dispatcher.h"
 #include "../db_handler.h"
#include "../db_constants.h"

class InfoNode  : public Node {

public:
	InfoNode();
	~InfoNode();
	void setRequestDispatcher(RequestDispatcher* rd);
protected:
	void executeGet(MgConnectionW& conn, const char* url);
private:
	RequestDispatcher* rd;

};

#endif /* INFO_NODE_H_ */
