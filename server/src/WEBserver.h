#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <string>
#include <vector>

using std::string;

#include "mongoose/mongoose.h"
#include "mgconnection.h"

extern "C" {
	#include <pthread.h>
}


/**
 * Clase que provee una abstraccion a mg_server, da la posibilidad de hacer un server con multiples threads.
 */
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
