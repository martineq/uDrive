/*
 * info_trash_node.h
 *
 *  Created on: 15/11/2015
 *      Author: martin
 */

#ifndef INFO_TRASH_NODE_H_
#define INFO_TRASH_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class InfoTrashNode : public Node {

public:
	InfoTrashNode(MgConnectionW& conn);
	~InfoTrashNode();
	void executeGet();
protected:

	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* INFO_TRASH_NODE_H_ */
