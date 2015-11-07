//
// Created by martin on 06/11/15.
//

#ifndef PROFILE_NODE_H_
#define PROFILE_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class ProfileNode : public Node {

public:
    ProfileNode(MgConnectionW& conn);
    ~ProfileNode();
protected:
    void executeGet();
    std::string defaultResponse();
    std::string getUserId();

private:
    vector<string> split(const string &s, char delim);


};

#endif /* PROFILE_NODE_H_ */

