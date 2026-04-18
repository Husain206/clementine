#pragma once
#include "utils.hpp"

struct Config {
  str save_file;
  str theme_name;

  vec<str> categories;
  str default_category;

  int sidebar_width = 18;
  int info_width = 28;

  bool compact_header = true;
};

Config load_config(const str& path);
