#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include <string>
#include <vector>

using std::string;

#include "mongoose/mongoose.h"
#include "util/log.h"
#include "rest/token_node.h"
#include "rest/info_node.h"
#include "rest/receive_file_node.h"

extern "C" {
	#include <pthread.h>
}

class WEBServer {
	public:

		WEBServer();
		~WEBServer();

		const char* setPort(string port);
		const char* setDocumentRoot(string path);
		void run();
		void stop();

	protected:
		struct mg_server *server;
		pthread_t hilo;
		static void* threadHandler(void*);
		static int handlerCaller(struct mg_connection*, enum mg_event);
		int running;

};

#endif
