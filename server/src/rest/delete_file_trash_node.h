/*
 * delete_file_trash_node.h
 *
 *  Created on: 15/11/2015
 *      Author: martin
 */

#ifndef DELETE_FILE_TRASH_NODE_H
#define DELETE_FILE_TRASH_NODE_H

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class DeleteFileTrashNode : public Node {

public:
	DeleteFileTrashNode(MgConnectionW& conn);
	~DeleteFileTrashNode();
	void executeDelete();
protected:

	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* DELETE_FILE_TRASH_NODE_H */
