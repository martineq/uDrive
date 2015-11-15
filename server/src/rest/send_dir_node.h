/*
 * send_dir_node.h
 *
 *  Created on: 26/10/2015
 *      Author: martin
 */

#ifndef SEND_DIR_NODE_H_
#define SEND_DIR_NODE_H_
#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
using std::vector;

class SendDirNode  : public Node {

public:
	SendDirNode(MgConnectionW& conn);
	~SendDirNode();
protected:
	void executeGet();
	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);
};

#endif /* SEND_DIR_NODE_H_ */
