#include <fstream>
#include "../include/theme.hpp"
#include <algorithm>
#include <ncurses.h>

Theme make_default_theme(){
  Theme t;
  t.name = "default";
  t.bg = -1;
  t.text = COLOR_WHITE;
  t.dim = COLOR_BLUE;
  t.border = COLOR_CYAN;
  t.border_active = COLOR_YELLOW;
  t.accent = COLOR_YELLOW;
  t.accent2 = COLOR_CYAN;
  t.warning = COLOR_RED;
  t.success = COLOR_GREEN;
  t.selected_fg = COLOR_BLACK;
  t.selected_bg = COLOR_CYAN;

  t.panel_title_color = COLOR_YELLOW;
  t.dialog_accent_color = COLOR_YELLOW;

  t.heavy_borders = true;
  t.use_shadow = true;
  t.dense_header = true;
  t.all_caps = true;
  t.show_ascii_logo = false;
  t.scanlines = false;
  t.double_borders = false;

  t.header_style = HeaderStyle::Classic;
  t.footer_style = FooterStyle::Classic;

  t.header_label = "TASK-NODE";
  t.footer_label = "SYS/READY";
  return t;
}

HeaderStyle header_style_from_name(const str& s){
  str v = to_lower(trim(s));
  if(v == "compact") return HeaderStyle::Compact;
  return HeaderStyle::Classic;
}

FooterStyle footer_style_from_name(const str& s){
  str v = to_lower(trim(s));
  if(v == "statusbar") return FooterStyle::StatusBar;
  return FooterStyle::Classic;
}


void apply_theme(const Theme &theme){
  if(!has_colors()) return;

  start_color();
  use_default_colors();

  init_pair(PAIR_BORDER,        theme.border,              theme.bg);
  init_pair(PAIR_ACCENT,        theme.accent,              theme.bg);
  init_pair(PAIR_TEXT,          theme.text,                theme.bg);
  init_pair(PAIR_SELECTED,      theme.selected_fg,         theme.selected_bg);
  init_pair(PAIR_COMPLETED,     theme.success,             theme.bg);
  init_pair(PAIR_WARNING,       theme.warning,             theme.bg);
  init_pair(PAIR_DIM,           theme.dim,                 theme.bg);
  init_pair(PAIR_PANEL_TITLE,   theme.panel_title_color,   theme.bg);
  init_pair(PAIR_DIALOG_ACCENT, theme.dialog_accent_color, theme.bg);}

short color_from_name(const str& s){
  str v = to_lower(s);
  if (v == "black")   return COLOR_BLACK;
  if (v == "red")     return COLOR_RED;
  if (v == "green")   return COLOR_GREEN;
  if (v == "yellow")  return COLOR_YELLOW;
  if (v == "blue")    return COLOR_BLUE;
  if (v == "magenta") return COLOR_MAGENTA;
  if (v == "cyan")    return COLOR_CYAN;
  if (v == "white")   return COLOR_WHITE;
  if (v == "default") return -1;

  bool numeric = !v.empty() && std::all_of(v.begin(), v.end(), ::isdigit);
  if (numeric) return (short)std::stoi(v);

  return COLOR_WHITE;
}

str make_theme_path(const str &theme_name){
  return "config/theme/" + theme_name + ".conf";
}

Theme load_theme_file(const str& path, Theme base){
  std::ifstream in(path);
  if(!in){
    return base;
  }

  str line;
  while(std::getline(in, line)){
    line = trim(line);

    if(line.empty() || line[0] == '#') continue;

    size_t pos = line.find('=');
    if(pos == str::npos) continue;

    str key = to_lower(trim(line.substr(0, pos)));
    str value = trim(line.substr(pos + 1));

    if(key == "bg_color") base.bg = color_from_name(value);
    else if(key == "text_color") base.text = color_from_name(value);
    else if(key == "dim_color") base.dim = color_from_name(value);
    else if(key == "border_color") base.border = color_from_name(value);
    else if(key == "border_active_color") base.border_active = color_from_name(value);
    else if(key == "accent_color") base.accent = color_from_name(value);
    else if(key == "accent2_color") base.accent2 = color_from_name(value);
    else if(key == "warning_color") base.warning = color_from_name(value);
    else if(key == "success_color") base.success = color_from_name(value);
    else if(key == "selected_fg") base.selected_fg = color_from_name(value);
    else if(key == "selected_bg") base.selected_bg = color_from_name(value);
    
    else if(key == "panel_title_color") base.panel_title_color = color_from_name(value);
    else if(key == "dialog_accent_color") base.dialog_accent_color = color_from_name(value);
    
    else if(key == "heavy_borders") base.heavy_borders = parse_bool(value);
    else if(key == "use_shadow") base.use_shadow = parse_bool(value);
    else if(key == "dense_header") base.dense_header = parse_bool(value);
    else if(key == "all_caps") base.all_caps = parse_bool(value);
    else if(key == "show_ascii_logo") base.show_ascii_logo = parse_bool(value);
    else if(key == "scanlines") base.scanlines = parse_bool(value);
    else if(key == "double_borders") base.double_borders = parse_bool(value);
    
    else if(key == "header_style") base.header_style = header_style_from_name(value);
    else if(key == "footer_style") base.footer_style = footer_style_from_name(value);
    
    else if(key == "header_label") base.header_label = value;
    else if(key == "footer_label") base.footer_label = value;
    else if(key == "theme_name") base.name = value;  }

  return base;
}
