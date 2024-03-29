#include "web_server.h"
#include "rest/delete_dir_node.h"
#include "rest/list_tags_node.h"
#include "rest/update_tags_node.h"
#include "rest/list_tags_user_node.h"
#include "rest/update_collaborators_node.h"
#include "rest/list_collaborators_node.h"
#include "rest/search_users_node.h"
#include "rest/search_filename_node.h"
#include "rest/search_extension_node.h"
#include "rest/search_tag_node.h"
#include "rest/search_owner_node.h"
#include "rest/list_owners_node.h"
#include "rest/list_info_elem_node.h"
#include "rest/update_location_node.h"
#include "rest/modify_name_dir_node.h"

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
      Log(Log::LogMsgInfo) << "[TokenNode]";
      TokenNode* tn=new TokenNode(mgConnection);
    tn->setRequestDispatcher(RequestDispatcher::get_instance());
      tn->execute();
    return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/info/users",11)) {
      vector<string> lista = WEBServer::split(conn->uri, '/');
      string field = lista[4];
      Log(Log::LogMsgDebug) << "[" << "URI: /info/users" << "], field: " <<field << " Method: "<<conn->request_method <<", Param fileIds: "<<mgConnection.getParameter("fileIds");

      if ( ( field == "trash") and (!strncmp(mgConnection.getMethod(),"GET",3)) and  ( lista.size()==5)){
          Log(Log::LogMsgInfo) << "[InfoTrashNode]";
          InfoTrashNode * itn=new InfoTrashNode(mgConnection);
          itn->setRequestDispatcher(RequestDispatcher::get_instance());
          itn->execute();
          delete itn;
          return MG_TRUE;

      }else if ( ( field == "dir") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){
          Log(Log::LogMsgInfo) << "[InfoNode]";
          InfoNode * in=new InfoNode(mgConnection);
          in->setRequestDispatcher(RequestDispatcher::get_instance());
          in->execute();
          delete in;
          return MG_TRUE;

      }else if ( ( field == "file") and (!strncmp(mgConnection.getMethod(),"GET",3)) and (mgConnection.getParameter("name")!="") ){
          Log(Log::LogMsgInfo) << "[SearchFilenameNode]";
          SearchFilenameNode * in=new SearchFilenameNode(mgConnection);
          in->setRequestDispatcher(RequestDispatcher::get_instance());
          in->execute();
          delete in;
          return MG_TRUE;

      }else if ( ( field == "file") and (!strncmp(mgConnection.getMethod(),"GET",3)) and (mgConnection.getParameter("extension")!="") ){
          Log(Log::LogMsgInfo) << "[SearchExtensionNode]";
          SearchExtensionNode * in=new SearchExtensionNode(mgConnection);
          in->setRequestDispatcher(RequestDispatcher::get_instance());
          in->execute();
          delete in;
          return MG_TRUE;

      }else if ( ( field == "tags") and (!strncmp(mgConnection.getMethod(),"GET",3)) and (mgConnection.getParameter("tagname")!="") ){
          Log(Log::LogMsgInfo) << "[SearchTagNode]";
          SearchTagNode * in=new SearchTagNode(mgConnection);
          in->setRequestDispatcher(RequestDispatcher::get_instance());
          in->execute();
          delete in;
          return MG_TRUE;

      }else if ( ( field == "owners") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){
          Log(Log::LogMsgInfo) << "[SearchOwnerNode]";
          SearchOwnerNode * in=new SearchOwnerNode(mgConnection);
          in->setRequestDispatcher(RequestDispatcher::get_instance());
          in->execute();
          delete in;
          return MG_TRUE;

      }else if ( ( field == "trash") and (!strncmp(mgConnection.getMethod(),"DELETE",6)) and (mgConnection.getParameter("fileIds")=="") ){
          Log(Log::LogMsgInfo) << "[DeleteTrashNode]";
          DeleteTrashNode * dtn=new DeleteTrashNode(mgConnection);
          dtn->setRequestDispatcher(RequestDispatcher::get_instance());
          dtn->execute();
          delete dtn;
          return MG_TRUE;

      }else if ( ( field == "trash") and (!strncmp(mgConnection.getMethod(),"DELETE",6)) ){
          Log(Log::LogMsgInfo) << "[DeleteFileTrashNode]";
          DeleteFileTrashNode * dftn=new DeleteFileTrashNode(mgConnection);
          dftn->setRequestDispatcher(RequestDispatcher::get_instance());
          dftn->execute();
          delete dftn;
          return MG_TRUE;

      }else if (( field == "trash") and (!strncmp(mgConnection.getMethod(),"POST",4)) and (mgConnection.getParameter("fileIds")!="") ){
          Log(Log::LogMsgInfo) << "[RestoreFileTrashNode]";
          RestoreFileTrashNode * rftn=new RestoreFileTrashNode(mgConnection);
          rftn->setRequestDispatcher(RequestDispatcher::get_instance());
          rftn->execute();
          delete rftn;
          return MG_TRUE;
      }else if ( ( field == "trash") and (!strncmp(mgConnection.getMethod(),"POST",4)) and  ( lista.size()>4)){
          Log(Log::LogMsgInfo) << "[RestoreTrashNode]";
          RestoreTrashNode * rtn=new RestoreTrashNode(mgConnection);
          rtn->setRequestDispatcher(RequestDispatcher::get_instance());
          rtn->execute();
          delete rtn;
          return MG_TRUE;

      }else return MG_FALSE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/file/users",11)) {
    Log(Log::LogMsgInfo) << "[ReceiveFileNode]";
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
          Log(Log::LogMsgInfo) << "[DeleteFileNode]";
          DeleteFileNode* dfn=new DeleteFileNode(mgConnection);
          dfn->setRequestDispatcher(RequestDispatcher::get_instance());
          dfn->execute();
          delete dfn;
          return MG_TRUE;
      }else if ( ( field == "dir") and (!strncmp(mgConnection.getMethod(),"DELETE",6)) ){
          Log(Log::LogMsgInfo) << "[DeleteDirNode]";
          DeleteDirNode* ddn=new DeleteDirNode(mgConnection);
          ddn->setRequestDispatcher(RequestDispatcher::get_instance());
          ddn->execute();
          delete ddn;
          return MG_TRUE;

      }else if ( ( field == "dir") and (!strncmp(mgConnection.getMethod(),"POST",4)) ){
          Log(Log::LogMsgInfo) << "[CreateDirNode]";
          CreateDirNode* cdn=new CreateDirNode(mgConnection);
          cdn->setRequestDispatcher(RequestDispatcher::get_instance());
          cdn->execute();
          delete cdn;
          return MG_TRUE;

      }else if ( ( field == "file") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){
          Log(Log::LogMsgInfo) << "[SendFileNode]";
          SendFileNode* sfn=new SendFileNode(mgConnection);
          sfn->setRequestDispatcher(RequestDispatcher::get_instance());
          sfn->execute();
          delete sfn;
          return MG_TRUE;

      }else if ( ( field == "dir") and (!strncmp(mgConnection.getMethod(),"GET",3)) ){
          Log(Log::LogMsgInfo) << "[SendDirNode]";
          SendDirNode* sfn=new SendDirNode(mgConnection);
          sfn->setRequestDispatcher(RequestDispatcher::get_instance());
          sfn->execute();
          delete sfn;
          return MG_TRUE;

      }else return MG_FALSE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/signup",7)) {
      Log(Log::LogMsgInfo) << "[SignupNode]";
    SignupNode* sn=new SignupNode(mgConnection);
    sn->setRequestDispatcher(RequestDispatcher::get_instance());
      sn->execute();
    delete sn;
    return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/profile",8)) {
      Log(Log::LogMsgInfo) << "[ProfileNode]";
      ProfileNode* sn=new ProfileNode(mgConnection);
      sn->setRequestDispatcher(RequestDispatcher::get_instance());
      sn->execute();
      delete sn;
      return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/photo",6)) {
      Log(Log::LogMsgInfo) << "[UpdatePhotoNode]";
      UpdatePhotoNode* upn=new UpdatePhotoNode(mgConnection);
      upn->setRequestDispatcher(RequestDispatcher::get_instance());
      upn->execute();
      delete upn;
      return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/userfullname",12)) {
      Log(Log::LogMsgInfo) << "[UpdateUserFullNameNode]";
      UpdateUserFullNameNode* uun=new UpdateUserFullNameNode(mgConnection);
      uun->setRequestDispatcher(RequestDispatcher::get_instance());
      uun->execute();
      delete uun;
      return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/filetags/users/",16)) {

      vector<string> lista = WEBServer::split(conn->uri, '/');

      if (!strncmp(mgConnection.getMethod(),"GET",3) and (lista.size()!=4)){
          Log(Log::LogMsgInfo) << "[ListTagsNode]";
          ListTagsNode* ltn=new ListTagsNode(mgConnection);
          ltn->setRequestDispatcher(RequestDispatcher::get_instance());
          ltn->execute();
          delete ltn;
          return MG_TRUE;

      }else if (!strncmp(mgConnection.getMethod(),"GET",3) and (lista.size()==4)){
          Log(Log::LogMsgInfo) << "[ListTagsUserNode]";
          ListTagsUserNode* ltn=new ListTagsUserNode(mgConnection);
          ltn->setRequestDispatcher(RequestDispatcher::get_instance());
          ltn->execute();
          delete ltn;
          return MG_TRUE;

      }else if (!strncmp(mgConnection.getMethod(),"PUT",3)){
          Log(Log::LogMsgInfo) << "[UpdateTagsNode]";
          UpdateTagsNode* uun=new UpdateTagsNode(mgConnection);
          uun->setRequestDispatcher(RequestDispatcher::get_instance());
          uun->execute();
          delete uun;
          return MG_TRUE;
      } else return MG_FALSE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/revisions/users/",17)) {
      Log(Log::LogMsgDebug) << "[OBTIENE LISTA DE REVISIONES DE UN ARCHIVO]";
      Log(Log::LogMsgInfo) << "[NO IMPLEMENTADO]";

      return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/fileInfo/user/",15)) {

      Log(Log::LogMsgDebug) << "[/fileInfo/user/]";

      if (!strncmp(mgConnection.getMethod(),"GET",3)){
          Log(Log::LogMsgInfo) << "[ListCollaboratorsNode]";
          ListCollaboratorsNode* uun=new ListCollaboratorsNode(mgConnection);
          uun->setRequestDispatcher(RequestDispatcher::get_instance());
          uun->execute();
          delete uun;
          return MG_TRUE;

      }else if (!strncmp(mgConnection.getMethod(),"POST",4)){
          Log(Log::LogMsgInfo) << "[UpdateCollaboratorsNode]";
          UpdateCollaboratorsNode* uun=new UpdateCollaboratorsNode(mgConnection);
          uun->setRequestDispatcher(RequestDispatcher::get_instance());
          uun->execute();
          delete uun;
          return MG_TRUE;
      }else return MG_FALSE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/owners/users/",14)) {
      Log(Log::LogMsgInfo) << "[ListOwnersNode]";
      ListOwnersNode* uun=new ListOwnersNode(mgConnection);
      uun->setRequestDispatcher(RequestDispatcher::get_instance());
      uun->execute();
      delete uun;
      return MG_TRUE;
  }else if ( (ev == MG_REQUEST && !strncmp(conn->uri, "/user/",6))  and (mgConnection.getParameter("q")!="") ){
      Log(Log::LogMsgInfo) << "[SearchUsersNode]";
      SearchUsersNode* sfn=new SearchUsersNode(mgConnection);
      sfn->setRequestDispatcher(RequestDispatcher::get_instance());
      sfn->execute();
      delete sfn;
      return MG_TRUE;
  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/fileinfo/users/",16)) {
      Log(Log::LogMsgInfo) << "[ListInfoElemNode]";
      ListInfoElemNode* sfn=new ListInfoElemNode(mgConnection);
      sfn->setRequestDispatcher(RequestDispatcher::get_instance());
      sfn->execute();
      delete sfn;
      return MG_TRUE;

  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/location/users/",16)) {
      Log(Log::LogMsgInfo) << "[UpdateLocationNode]";
      UpdateLocationNode* sfn=new UpdateLocationNode(mgConnection);
      sfn->setRequestDispatcher(RequestDispatcher::get_instance());
      sfn->execute();
      delete sfn;
      return MG_TRUE;
  } else if (ev == MG_REQUEST && !strncmp(conn->uri, "/filename/users/",16)) {
      Log(Log::LogMsgInfo) << "[ModifyNameDirNode]";
      ModifyNameDirNode* sfn=new ModifyNameDirNode(mgConnection);
      sfn->setRequestDispatcher(RequestDispatcher::get_instance());
      sfn->execute();
      delete sfn;
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