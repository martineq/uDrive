extern "C" {
	#include <signal.h>
	#include <getopt.h>
	#include <unistd.h>
}

#include <iostream>
#include <string>
#include <fstream>
#include "web_server.h"
#include "util/log.h"

using std::cout;
using std::endl;

sig_atomic_t Corriendo = 1;

void sig_handler(int){
	Corriendo = 0;
}

typedef struct Configuration {
	Configuration() : port("8080"), logFile("-"), logLevel("debug"),dbPath("/tmp/test_db")
	{}
	string port;
	string logFile;
	string logLevel;
	string dbPath;
} Configuration;

int parse_cmd(int argc, char *argv[], Configuration& config){
	opterr = 0;
	int c;

	static struct option long_options[] = {
		{"port", required_argument, 0, 'p'},
		{"log-file", required_argument, 0, 'l'},
		{"log-level", required_argument, 0, 'L'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	int option_index = 0;

	while((c = getopt_long (argc, argv, "hp:l:L:", long_options, &option_index))){
		if (c == -1)
			break;

		switch (c){
			case 'l':
				config.logFile = optarg;
				break;

			case 'L':
				config.logLevel = optarg;
				break;

			case 'd':
				config.dbPath = optarg;
				break;

			case 'p':
				config.port = optarg;
				break;
			case 'h':
				std::cout << "-l: The log file path indicated. (default: stdout)"<< std::endl;
				std::cout << "-L: It indicated in verbose level. (default: info)"<< std::endl;
				std::cout << "-d: The base path rockdb. (default: /tmp/test_db)"<< std::endl;
				std::cout << "-p: The port where the service indicated listen. (default: 8080)"<< std::endl;
				std::cout << "-h: Help. "<< std::endl;
				break;

			default:
				std::cout << "not valid param"<< std::endl;
				return -1;
				break;
		}
	}

	return 0;
}
 
int main(int argc, char** argv) {
		Configuration config;
		//Tomar configuracion por parametros.
			if(parse_cmd(argc, argv, config))
				return -1;

			std::cout << config.logFile << std::endl;
		//Codigo para que la aplicacion cierre bien.
		signal(SIGHUP, sig_handler);
		signal(SIGQUIT, sig_handler);
		signal(SIGKILL, sig_handler);
		signal(SIGINT, sig_handler);

		//Set nivel de logueo.
		Log::setLogLevel(config.logLevel);
		std::ofstream outputLog;
		if(config.logFile != "-"){
			outputLog.open(config.logFile);
			if(outputLog.is_open())
				Log::setOutput(outputLog);
			else
				Log(Log::LogMsgError) << "Error opening file '" << config.logFile << "'";
		}
		// Inicializo servidor web
		Log(Log::LogMsgInfo) << "starting server...";
		Log(Log::LogMsgInfo) << "localhost: " << config.port;
		WEBServer server;
		server.setPort(config.port);
		server.run();
		Log(Log::LogMsgInfo) << "server started";
		while(Corriendo)
			sleep(1);
		Log(Log::LogMsgInfo) << "stopped server";
		server.stop();
	return 0;
}
