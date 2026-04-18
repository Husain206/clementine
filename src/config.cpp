#include "../include/config.hpp"
#include <fstream>
#include <string>

Config load_config(const str &path){
  Config config;
  std::ifstream input(path);
  if(!input){
    // return default config
    return config;
  }

  str line;
  while(std::getline(input, line)){
    line = trim(line);

    if(line.empty() || line[0] == '#') continue;

    const usize pos = line.find('=');
    if(pos == str::npos){
      continue;
    }
    str key = trim(line.substr(0, pos));
    str value = trim(line.substr(pos + 1));

    key = to_lower(key);

    if(key == "save_file")               config.save_file = value;
    else if(key == "theme_name")         config.theme_name = to_lower(value);
    else if(key == "default_catagory")   config.default_category = value;
    else if(key == "categories")         config.categories = split_csv(value);
    else if(key == "sidebar_width")      config.sidebar_width = std::stoi(value);
    else if(key == "info_width")         config.info_width = std::stoi(value);
    else if(key == "compact_header")     config.compact_header = parse_bool(value);

    // } else if(key == "use_256_colors"){
    //   config.use_256_colors = parse_bool(value);
  }
  return config;
}
