//
// Created by martin on 06/11/15.
//

#ifndef UPDATE_PHOTO_NODE_H_
#define UPDATE_PHOTO_NODE_H_

#include "../rest/node.h"
#include "../util/log.h"
#include <string.h>
#include "../request_dispatcher.h"
#include "../db_handler.h"
#include "../db_constants.h"

class UpdatePhotoNode : public Node {

public:
    UpdatePhotoNode(MgConnectionW& conn);
    ~UpdatePhotoNode();
protected:
    void executePut();
    std::string defaultResponse();
    std::string getUserId();
private:
    vector<string> split(const string &s, char delim);

};

#endif /* UPDATE_PHOTO_NODE_H_ */

