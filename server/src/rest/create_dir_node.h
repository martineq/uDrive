/*
 * create_dir_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef CREATE_DIR_NODE_H_
#define CREATE_DIR_NODE_H_
#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"
#include "../lib/json/json.h"
#include "../util/random_number.h"
#include "../util/md5.h"

class CreateDirNode  : public Node {

public:
	CreateDirNode(MgConnectionW& conn);
	~CreateDirNode();
protected:
	void executePost();
	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* CREATE_DIR_NODE_H_ */
