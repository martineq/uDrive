/*
 * restore_trash_node.h
 *
 *  Created on: 15/11/2015
 *      Author: martin
 */

#ifndef RESTORE_TRASH_NODE_H_
#define RESTORE_TRASH_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class RestoreTrashNode : public Node {

public:
	RestoreTrashNode(MgConnectionW& conn);
	~RestoreTrashNode();
	void executeDelete();
protected:

	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* RESTORE_TRASH_NODE_H_ */
