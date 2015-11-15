/*
 * delete_dir_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef DELETE_DIR_NODE_H_
#define DELETE_DIR_NODE_H_
#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"
#include "../lib/json/json.h"
#include "../util/random_number.h"
#include "../util/md5.h"
#include "info_node.h"


class DeleteDirNode  : public Node {

public:
	DeleteDirNode(MgConnectionW& conn);
	~DeleteDirNode();
protected:
	void executeDelete();
	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);
};

#endif /* DELETE_DIR_NODE_H_ */
