/*
 * restore_file_trash_node.h
 *
 *  Created on: 15/11/2015
 *      Author: martin
 */

#ifndef RESTORE_FILE_TRASH_NODE_H
#define RESTORE_FILE_TRASH_NODE_H

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class RestoreFileTrashNode : public Node {

public:
	RestoreFileTrashNode(MgConnectionW& conn);
	~RestoreFileTrashNode();
	void executePost();
protected:

	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* RESTORE_FILE_TRASH_NODE_H */
