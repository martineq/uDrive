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
	ReceiveFileNode();
	~ReceiveFileNode();
	void setRequestDispatcher(RequestDispatcher* rd);
protected:
	void executePost(MgConnectionW& conn, const char* url);
private:
	vector<string> split(const string &s, char delim);
	RequestDispatcher* rd;
};

#endif /* RECEIVE_FILE_NODE_H_ */
