#ifndef YAMLPARSER_H
#define YAMLPARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>

#define YAML_CONFIG_FILE "config.yml"
#define YAML_LABEL_IP "ip"
#define YAML_LABEL_PORT "port"
#define YAML_EMPTY_STRING ""

class ConfigParser {
  
  public:
    
    struct stConnection{
      std::string ip;
      std::string port;
    };

    ConfigParser();
    ~ConfigParser();
    
    bool load_configuration(ConfigParser::stConnection& connection);
  
  private:
    
    void load_content(YAML::Node& root_node, ConfigParser::stConnection& connection);
    std::string read_yaml_node_to_string(const YAML::Node& node);
    void notify_read_error(std::string file, int line, std::string msg, bool& read_ok);
  
};

#endif // YAMLPARSER_H
