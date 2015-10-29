#include "web_server.h"
#include <string.h>
#include <sstream>
#include "../lib/json/json.h"


WEBServer::WEBServer(){
		server = mg_create_server(server, WEBServer::handlerCaller);
}

WEBServer::~WEBServer(){
	if(this->running)
		this->stop();
}
/**
 * Defime el port donde escucha el servidor. Por defecto 8000.
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
/**
 * Metodo que maneja los request del servidor.
 */
int WEBServer::handlerCaller(struct mg_connection *conn, enum mg_event ev){
	mg_server* mgServer = (mg_server*) conn->server_param;
	MgConnectionW mgConnection(conn);

  if (ev == MG_AUTH) {
	  Log(Log::LogMsgDebug) << "[" << conn->remote_ip << "] " << conn->request_method << " " << conn->uri << " " << conn->query_string;
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && !strcmp(conn->uri, "/token")) {
	  TokenNode* tn=new TokenNode();
	  tn->setRequestDispatcher(RequestDispatcher::getInstance());
	  tn->execute(mgConnection,conn->uri);
    return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/info/users",11)) {
 	  InfoNode* in=new InfoNode();
 	  in->setRequestDispatcher(RequestDispatcher::getInstance());
 	  in->execute(mgConnection,conn->uri);
     return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/file/users",11)) {
	  		ReceiveFileNode* rfn=new ReceiveFileNode();
 	  		rfn->setRequestDispatcher(RequestDispatcher::getInstance()); 
 	  		rfn->execute(mgConnection,conn->uri);
     return MG_TRUE; 

   } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/users",6)) {
	  		CreateDirNode* cdn=new CreateDirNode();
 	  		cdn->setRequestDispatcher(RequestDispatcher::getInstance()); 
 	  		cdn->execute(mgConnection,conn->uri);
     return MG_TRUE; 

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/signup",7)) {
 	  SignupNode* sn=new SignupNode();
 	  sn->setRequestDispatcher(RequestDispatcher::getInstance());
 	  sn->execute(mgConnection,conn->uri);
     return MG_TRUE; 
     
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


