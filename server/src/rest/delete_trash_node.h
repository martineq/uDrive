/*
 * delete_trash_node.h
 *
 *  Created on: 15/11/2015
 *      Author: martin
 */

#ifndef DELETE_TRASH_NODE_H_
#define DELETE_TRASH_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class DeleteTrashNode : public Node {

public:
	DeleteTrashNode(MgConnectionW& conn);
	~DeleteTrashNode();
	void executeDelete();
protected:

	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* DELETE_TRASH_NODE_H_ */
