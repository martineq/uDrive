/*
 * search_tag_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef SEARCH_TAG_NODE_H_
#define SEARCH_TAG_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class SearchTagNode : public Node {

public:
	SearchTagNode(MgConnectionW& conn);
	~SearchTagNode();
	void executeGet();
protected:
	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* SEARCH_TAG_NODE_H_ */
