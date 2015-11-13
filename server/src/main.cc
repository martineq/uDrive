extern "C" {
	#include <signal.h>
	#include <getopt.h>
	#include <unistd.h>
}

#include <string>
#include <fstream>
#include "web_server.h"

using std::cout;
using std::endl;

sig_atomic_t Corriendo = 1;

void sig_handler(int){
	Corriendo = 0;
}

int main(int argc, char** argv) {
		ConfigParser::Configuration config;
		int result=ConfigParser::takeConfFromFile(config);
		if (result!=0){
			std::cout << "Init server fail!" << std::endl;
			return result;
		}
		std::ofstream outputLog;
		if(config.logfile != "-"){
			outputLog.open(config.logfile);
			if(outputLog.is_open()) Log::setOutput(outputLog);
		}
		Log::setLogLevel(config.loglevel);

		Log(Log::LogMsgInfo) << "Bindport: " << config.bindport;
		Log(Log::LogMsgInfo) << "Bindip: " << config.bindip;
		Log(Log::LogMsgInfo) << "Loglevel: " << config.loglevel;
		Log(Log::LogMsgInfo) << "Logfile: " << config.logfile;

		Log(Log::LogMsgInfo) << "Dbpath: " << config.dbpath;
		Log(Log::LogMsgInfo) << "QuoteUser: " << config.maxquotauser;
		Log(Log::LogMsgInfo) << "Starting server...";

		//close signals
		signal(SIGHUP, sig_handler);
		signal(SIGQUIT, sig_handler);
		signal(SIGKILL, sig_handler);
		signal(SIGINT, sig_handler);
		
		// Init web server
		WEBServer* server=new WEBServer();
		server->setPort(config.bindport);
		server->setDbPath(config.dbpath);
		server->setQuotaUser(config.maxquotauser);
		server->run();

		Log(Log::LogMsgInfo) << "Server started";
		while(Corriendo)
			sleep(1);
		Log(Log::LogMsgInfo) << "Stopped server";
		server->stop();
	return 0;
}
