#include "config_parser.h"
#include "log.h"

ConfigParser::ConfigParser(){

}
ConfigParser::~ConfigParser(){

}
int ConfigParser::takeConfFromFile(ConfigParser::Configuration& config){
  //Take params from yml file
  ConfigParser yp;
  if (!yp.load_configuration(config)){
    std::cout << "Load configuration fail: not open config.yml." << std::endl;
    return 1;
  }else {
    std::cout << "Read configuration ok" << std::endl;
    return 0;
  }
}

bool ConfigParser::load_configuration(ConfigParser::Configuration& config){
  // Open file
  std::ifstream config_file(YAML_CONFIG_FILE);
  bool status_ok = true;
  Log(Log::LogMsgDebug) << "Loading config config.yml";

  // Parsing file
  if(config_file.good()==true){
    
    try{
      // Starts to read YAML file
      YAML::Parser parser(config_file);
      
      // Creates and obtains the root node of YAML file
      YAML::Node root_node;
      parser.GetNextDocument(root_node);
      
      // Reads the content
      load_content(root_node,config); }catch(YAML::Exception& e){ notify_read_error(__FILE__,__LINE__,e.what(),status_ok); }
    
  }else{ notify_read_error(__FILE__,__LINE__,"Error opening config file. ",status_ok); return false; }
  
  return status_ok;
}


void ConfigParser::load_content(YAML::Node& root_node, ConfigParser::Configuration& config){
  Log(Log::LogMsgDebug) << "Reading content config.yml";

  // Iterate nodes, reading information
  for(YAML::Iterator it=root_node.begin() ; it!=root_node.end() ; ++it){

    std::string key;
    key = read_yaml_node_to_string(it.first());
    
    if( key.compare(YAML_LABEL_BINDIP) == 0 ){ config.bindip = read_yaml_node_to_string(it.second());
    }else if ( key.compare(YAML_LABEL_BINDPORT) == 0 ){ config.bindport = read_yaml_node_to_string(it.second());
    }else if ( key.compare(YAML_LABEL_LOGFILE) == 0 ){ config.logfile = read_yaml_node_to_string(it.second());
    }else if ( key.compare(YAML_LABEL_LOGLEVEL) == 0 ){ config.loglevel = read_yaml_node_to_string(it.second()); 
    }else if ( key.compare(YAML_LABEL_DBPATH) == 0 ){ config.dbpath = read_yaml_node_to_string(it.second());
    }else if ( key.compare(YAML_LABEL_MAXQUOTAUSER) == 0 ){ config.maxquotauser = stoul(read_yaml_node_to_string(it.second()),nullptr,10);
    }
  }

  return void();
}


std::string ConfigParser::read_yaml_node_to_string(const YAML::Node& node){
  std::string value;
  bool read_ok = true;
  
  // Reads the value
  try{
    node >> value;
  }catch(YAML::Exception& e){ notify_read_error(__FILE__,__LINE__,e.what(),read_ok); }

  if(read_ok == false){ value.assign(YAML_EMPTY_STRING); }
  
  return value;
}

void ConfigParser::notify_read_error(std::string file, int line, std::string msg, bool& read_ok){
  std::string message_error("YAML sintax error. Report: ");
  message_error.append(msg);
  std::cerr << message_error << std::endl;
  std::cerr <<"file: " << file << " - line: "<< line << message_error << std::endl;
  Log(Log::LogMsgError) << message_error;
  read_ok = false;
}
