extern "C" {
	#include <signal.h>
	#include <getopt.h>
	#include <unistd.h>
}

#include <string>
#include <fstream>
#include "web_server.h"
#include "util/log.h"
#include "config_parser.h"
#include "request_dispatcher.h"

using std::cout;
using std::endl;

sig_atomic_t Corriendo = 1;

void sig_handler(int){
	Corriendo = 0;
}

void takeConfFromFile(ConfigParser::Configuration& config){
	//Take params from yml file
		ConfigParser yp;
		if (!yp.load_configuration(config)){
  			std::cout << "Load configuration fail" << std::endl;
  		}
}

int main(int argc, char** argv) {
		ConfigParser::Configuration config;
		takeConfFromFile(config);

		//Set nivel de logueo.
		std::cout << config.loglevel << std::endl;
		Log::setLogLevel(config.loglevel);

		Log(Log::LogMsgInfo) << "Starting server...";
		Log(Log::LogMsgInfo) << "Bindport: " << config.bindport;
		Log(Log::LogMsgInfo) << "Bindip: " << config.bindip;
		Log(Log::LogMsgInfo) << "Loglevel: " << config.loglevel;
		Log(Log::LogMsgInfo) << "Logfile: " << config.logfile;
		Log(Log::LogMsgInfo) << "Dbpath: " << config.dbpath;

		//close signals
		signal(SIGHUP, sig_handler);
		signal(SIGQUIT, sig_handler);
		signal(SIGKILL, sig_handler);
		signal(SIGINT, sig_handler);
		
		//Set output log
		std::ofstream outputLog;
		if(config.logfile != "-"){
			outputLog.open(config.logfile);
			if(outputLog.is_open())
				Log::setOutput(outputLog);
			else
				Log(Log::LogMsgError) << "Error opening file '" << config.logfile << "'";
		}

		//Init DB
		Log(Log::LogMsgInfo) << "Init database...";
		RequestDispatcher rd;
  		string db_path = "/tmp/testdb_checkpoint2";
 		size_t max_quota = 9999;
  		if (!rd.init(db_path,max_quota)) {
  			Log(Log::LogMsgError) << "Init database fail";
  			return 0;
  		}

		// Init web server
		WEBServer server;
		server.setPort(config.bindport);
		server.setRequestDispatcher(&rd);
		server.run();
		Log(Log::LogMsgInfo) << "Server started";
		while(Corriendo)
			sleep(1);
		Log(Log::LogMsgInfo) << "Stopped server";
		server.stop();
	return 0;
}
