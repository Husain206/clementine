#pragma once
#include "utils.hpp"

struct Config {
  str save_file = "data/clementine.txt";
  str theme_name = "default";
  bool show_completed = true;
  bool show_archived = false;
  str default_catagory = "general";
  bool use_256_colors = true;
  str date_format = "%Y-%m-%d %H:%M";
};

Config load_config(const str& path);
