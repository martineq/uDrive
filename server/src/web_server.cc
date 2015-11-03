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
	//Log(Log::LogMsgInfo) << "[" << conn->remote_ip << "] " << conn->request_method << " " << conn->uri << " " << conn->query_string;

  if (ev == MG_AUTH) {
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && !strcmp(conn->uri, "/token")) {
	  TokenNode* tn=new TokenNode();
	  tn->setRequestDispatcher(RequestDispatcher::get_instance("db_test",9999)); // TODO(martindonofrio): change hardcoded values
	  tn->execute(mgConnection,conn->uri);
    return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/info/users",11)) {
 	  InfoNode* in=new InfoNode();
 	  in->setRequestDispatcher(RequestDispatcher::get_instance("db_test",9999));  // TODO(martindonofrio): change hardcoded values
 	  in->execute(mgConnection,conn->uri);
     return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/file/users",11)) {
	  		ReceiveFileNode* rfn=new ReceiveFileNode();
 	  		rfn->setRequestDispatcher(RequestDispatcher::get_instance("db_test",9999)); // TODO(martindonofrio): change hardcoded values
 	  		rfn->execute(mgConnection,conn->uri);
     return MG_TRUE; 

   } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/users",6)) {
	  		CreateDirNode* cdn=new CreateDirNode();
 	  		cdn->setRequestDispatcher(RequestDispatcher::get_instance("db_test",9999)); // TODO(martindonofrio): change hardcoded values
 	  		cdn->execute(mgConnection,conn->uri);
     return MG_TRUE; 

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/signup",7)) {
		 	  SignupNode* sn=new SignupNode();
		 	  sn->setRequestDispatcher(RequestDispatcher::get_instance("db_test",9999)); // TODO(martindonofrio): change hardcoded values
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


