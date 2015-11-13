#include "web_server.h"

WEBServer::WEBServer(){
		server = mg_create_server(server, WEBServer::handlerCaller);
}

WEBServer::~WEBServer(){
	if(this->running)
		this->stop();
}
/**
 * Defines the port where the server listens.
 */
const char* WEBServer::setPort(string port){
	return mg_set_option(server, "listening_port", port.c_str());
}

void WEBServer::run(){
	this->running = 1;
	pthread_create(&hilo, NULL, WEBServer::threadHandler, this);
}
/**
 * Method that handles server request.
 */
int WEBServer::handlerCaller(struct mg_connection *conn, enum mg_event ev){
    MgConnectionW mgConnection(conn);

   Log(Log::LogMsgInfo) << "[" << conn->remote_ip << "] " << conn->request_method << " " << conn->uri << " " << conn->query_string;

  if (ev == MG_AUTH) {
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && !strcmp(conn->uri, "/token")) {
    TokenNode* tn=new TokenNode(mgConnection);
    tn->setRequestDispatcher(RequestDispatcher::get_instance());
      tn->execute();
    return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/info/users",11)) {
      vector<string> lista = WEBServer::split(conn->uri, '/');
      string field = lista[4];
      Log(Log::LogMsgDebug) << "[" << "URI: /users" << "], field: " <<field <<" Method: "<<conn->request_method;
      if ( ( field == "trash") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){


      }else if ( ( field == "") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){
          InfoNode * in=new InfoNode(mgConnection);
          in->setRequestDispatcher(RequestDispatcher::get_instance());
          in->execute();
          delete in;
          return MG_TRUE;
      }else return MG_FALSE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/file/users",11)) {
    ReceiveFileNode* rfn=new ReceiveFileNode(mgConnection);
    rfn->setRequestDispatcher(RequestDispatcher::get_instance());
     rfn->execute();
    delete rfn;
    return MG_TRUE;

   } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/users",6)) {

      vector<string> lista = WEBServer::split(conn->uri, '/');
      string field = lista[3];

      Log(Log::LogMsgDebug) << "[" << "URI: /users" << "], field: " <<field <<" Method: "<<conn->request_method;

      if ( ( field == "file") and (!strncmp(mgConnection.getMethod(),"DELETE",6)) ){
          DeleteFileNode* dfn=new DeleteFileNode(mgConnection);
          dfn->setRequestDispatcher(RequestDispatcher::get_instance());
          dfn->execute();
          delete dfn;
          return MG_TRUE;

      }else if ( ( field == "dir") and (!strncmp(mgConnection.getMethod(),"POST",4)) ){
          CreateDirNode* cdn=new CreateDirNode(mgConnection);
          cdn->setRequestDispatcher(RequestDispatcher::get_instance());
          cdn->execute();
          delete cdn;
          return MG_TRUE;

      }else if ( ( field == "file") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){
          SendFileNode* sfn=new SendFileNode(mgConnection);
          sfn->setRequestDispatcher(RequestDispatcher::get_instance());
          sfn->execute();
          delete sfn;
          return MG_TRUE;

      }else if ( ( field == "dir") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){
          SendDirNode* sfn=new SendDirNode(mgConnection);
          sfn->setRequestDispatcher(RequestDispatcher::get_instance());
          sfn->execute();
          delete sfn;
          return MG_TRUE;
      }else return MG_FALSE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/signup",7)) {
    SignupNode* sn=new SignupNode(mgConnection);
    sn->setRequestDispatcher(RequestDispatcher::get_instance());
      sn->execute();
    delete sn;
    return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/profile",8)) {
      ProfileNode* sn=new ProfileNode(mgConnection);
      sn->setRequestDispatcher(RequestDispatcher::get_instance());
      sn->execute();
      delete sn;
      return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/photo",6)) {
      UpdatePhotoNode* upn=new UpdatePhotoNode(mgConnection);
      upn->setRequestDispatcher(RequestDispatcher::get_instance());
      upn->execute();
      delete upn;
      return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/userfullname",12)) {
      UpdateUserFullNameNode* uun=new UpdateUserFullNameNode(mgConnection);
      uun->setRequestDispatcher(RequestDispatcher::get_instance());
      uun->execute();
      delete uun;
      return MG_TRUE;

  } else return MG_FALSE;  // Rest of the events are not processed
}

vector<string> WEBServer::split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void* WEBServer::threadHandler(void* arg){
	struct WEBServer* webserver = (struct WEBServer*) arg;
	while(webserver->running)
		mg_poll_server(webserver->server,1000);
	return NULL;
}

void WEBServer::stop(){
    this->running = 0;
    mg_destroy_server(&server);
}