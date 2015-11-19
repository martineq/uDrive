//
// Created by martin on 06/11/15.
//

#ifndef UPDATE_USERFULLNAME_NODE_H_
#define UPDATE_USERFULLNAME_NODE_H_

#include <string.h>
#include "../rest/node.h"
#include "../util/log.h"
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class UpdateUserFullNameNode : public Node {

public:
    UpdateUserFullNameNode(MgConnectionW& conn);
    ~UpdateUserFullNameNode();
protected:
    void executePut();
    std::string defaultResponse();
    std::string getUserId();
private:
    vector<string> split(const string &s, char delim);

};

#endif /* UPDATE_USERFULLNAME_NODE_H_ */

