//
// Created by martin on 06/11/15.
//

#ifndef UPDATE_PROFILE_NODE_H_
#define UPDATE_PROFILE_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class UpdateProfileNode : public Node {

public:
    UpdateProfileNode(MgConnectionW& conn);
    ~UpdateProfileNode();
protected:
    void executePut();
    std::string defaultResponse();
private:
    vector<string> split(const string &s, char delim);

};

#endif /* UPDATE_PROFILE_NODE_H_ */

