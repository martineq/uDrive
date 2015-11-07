/*
 * receive_file_node.h
 *
 *  Created on: 26/10/2015
 *      Author: martin
 */

#ifndef RECEIVE_FILE_NODE_H_
#define RECEIVE_FILE_NODE_H_
#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
using std::vector;
#include "../request_dispatcher.h"

class ReceiveFileNode  : public Node {

public:
	ReceiveFileNode(MgConnectionW& conn);
	~ReceiveFileNode();
protected:
	void executePost();
	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);
};

#endif /* RECEIVE_FILE_NODE_H_ */
