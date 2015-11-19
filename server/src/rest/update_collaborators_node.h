/*
 * update_collaborators_node.h
 *
 *  Created on: 4/10/2015
 *      Author: martin
 */

#ifndef UPDATE_COLLABORATORS_NODE_H_
#define UPDATE_COLLABORATORS_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class UpdateCollaboratorsNode : public Node {

public:
	UpdateCollaboratorsNode(MgConnectionW& conn);
	~UpdateCollaboratorsNode();
	void executePost();
protected:
	std::string defaultResponse();
	std::string getUserId();
private:
	vector<string> split(const string &s, char delim);

};

#endif /* UPDATE_COLLABORATORS_NODE_H_ */
