#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include <string>
#include <vector>
#include "mongoose/mongoose.h"
#include "util/log.h"
#include "rest/token_node.h"
#include "rest/info_node.h"
#include "rest/receive_file_node.h"
#include "rest/signup_node.h"
#include "rest/create_dir_node.h"
#include "rest/profile_node.h"
#include "rest/update_photo_node.h"
#include "rest/update_userfullname_node.h"
#include "rest/delete_file_node.h"
#include "rest/send_file_node.h"
#include "rest/send_dir_node.h"
#include "request_dispatcher.h"

extern "C" {
	#include <pthread.h>
}

using std::string;

class WEBServer {
	public:
		WEBServer();
		~WEBServer();

		const char* setPort(string port);
		void run();
		void stop();
        void setDbPath(std::string path);
        std::string getDbPath();

        void setQuotaUser(std::string quota);
        std::string getQuotaUser();

    protected:
		struct mg_server *server;
		pthread_t hilo;
		static void* threadHandler(void*);
		static int handlerCaller(struct mg_connection*, enum mg_event);
		int running;

	private:
		static vector<string> split(const string &s, char delim);
		string quotaUser="0";
		string dbPath="db_test";
	};

#endif
