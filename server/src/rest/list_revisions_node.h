/*
 * list_revisions_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef LIST_REVISIONS_NODE_H_
#define LIST_REVISIONS_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class ListRevisionsNode : public Node {

public:
	ListRevisionsNode(MgConnectionW& conn);
	~ListRevisionsNode();
	void executeGet();
protected:
	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* LIST_REVISIONS_NODE_H_ */
