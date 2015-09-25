#include "log.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::ostream;

ostream* Log::output = &cout;
Log::LogMsgType Log::logLevel = Log::LogMsgError;

Log::Log(Log::LogMsgType type) : level(type) {
}

Log::~Log(){
	(*Log::output) << this->ss.str() << endl;
}

void Log::setOutput(ostream& out){
	Log::output = &out;
}

void Log::setLogLevel(Log::LogMsgType level){
	Log::logLevel = level;
}

void Log::setLogLevel(const string& level){
	if(level == "debug")
		Log::logLevel = Log::LogMsgDebug;
	else if(level == "warning")
		Log::logLevel = Log::LogMsgWarning;
	else if(level == "info")
		Log::logLevel = Log::LogMsgInfo;
	else if(level == "error")
		Log::logLevel = Log::LogMsgError;
}
