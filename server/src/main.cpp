extern "C" {
	#include <signal.h>
	#include <getopt.h>
	#include <unistd.h>
}

#include <iostream>
#include <string>
#include <fstream>
#include "WEBserver.h"
#include "util/log.h"

std::string str;

using std::cout;
using std::endl;
using std::string;

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
 
int main(int argc, char** argv) {
		Configuration config;
		//Tomar configuracion por parametros.

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
				Log(Log::LogMsgError) << "Error abriendo archivo '" << config.logFile << "'";
		}

		// Inicializo servidor web
		Log(Log::LogMsgInfo) << "Iniciando Servidor...";
		Log(Log::LogMsgInfo) << "Port: " << config.port;
		WEBServer server;
		server.setPort(config.port);
		server.run();
		Log(Log::LogMsgInfo) << "Servidor iniciado";
		while(Corriendo)
			sleep(1);
		Log(Log::LogMsgInfo) << "Servidor finalizado";
		server.stop();
	return 0;
}

