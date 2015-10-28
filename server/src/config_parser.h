#ifndef YAMLPARSER_H
#define YAMLPARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "util/log.h"

#define YAML_CONFIG_FILE "config.yml"
#define YAML_LABEL_BINDIP "bindip"
#define YAML_LABEL_BINDPORT "bindport"
#define YAML_LABEL_LOGFILE "logfile"
#define YAML_LABEL_LOGLEVEL "loglevel"
#define YAML_LABEL_DBPATH "dbpath"
#define YAML_EMPTY_STRING ""

class ConfigParser {
  
  public:

    typedef struct Configuration {
      Configuration() : bindport("8080"), bindip("0.0.0.0"), logfile("-"), loglevel("debug"), dbpath("/tmp/db_test")
      {}
      std::string bindport;
      std::string bindip;
      std::string logfile;
      std::string loglevel;
      std::string dbpath;
    } Configuration;
    
    ConfigParser();
    ~ConfigParser();
    
    bool load_configuration(ConfigParser::Configuration& config);
  
  private:
    
    void load_content(YAML::Node& root_node, ConfigParser::Configuration& config);
    std::string read_yaml_node_to_string(const YAML::Node& node);
    void notify_read_error(std::string file, int line, std::string msg, bool& read_ok);
  
};

#endif // YAMLPARSER_H
