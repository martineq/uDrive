#include "WEBserver.h"
#include <string.h>

WEBServer::WEBServer(){
		server = mg_create_server(server, WEBServer::handlerCaller);
		mg_set_option(server, "listening_port", "8000");
}

WEBServer::~WEBServer(){
	if(this->running)
		this->stop();
}

/**
 * Defime el port donde escucha el servidor. Por defecto 8080.
 */
const char* WEBServer::setPort(string port){
	return mg_set_option(server, "listening_port", port.c_str());
}

/**
 * Defime el document root del servidor.
 */
const char* WEBServer::setDocumentRoot(string path){
	return mg_set_option(server, "document_root",path.c_str());
}

void WEBServer::run(){
	this->running = 1;
	pthread_create(&hilo, NULL, WEBServer::threadHandler, this);
}

int WEBServer::handlerCaller(struct mg_connection *conn, enum mg_event ev){
  if (ev == MG_AUTH) {
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && !strcmp(conn->uri, "/hello")) {
    mg_printf_data(conn, "%s", "Hello world");
    return MG_TRUE;   // Mark as processed
  } else {
    return MG_FALSE;  // Rest of the events are not processed
  }
}

void* WEBServer::threadHandler(void* arg){
	struct WEBServer * webserver = (struct WEBServer*) arg;
	while(webserver->running)
		mg_poll_server(webserver->server,1000);

	return NULL;
}

void WEBServer::stop(){
		this->running = 0;
		mg_destroy_server(&server);
}


