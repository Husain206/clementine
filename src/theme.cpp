#include "../include/theme.hpp"
#include "ncurses.h"

Theme make_default_theme(){
  return Theme{
    "default",
    COLOR_CYAN,   -1,  // border
    COLOR_YELLOW, -1,  // accent
    COLOR_WHITE,  -1,  // text
    COLOR_BLACK, COLOR_CYAN, // selected
    COLOR_GREEN,  -1,  // completed
    COLOR_RED, -1      // warning
  };
}

Theme make_green_theme() {
    return Theme{
        "green",
        COLOR_GREEN,   -1,
        COLOR_YELLOW,  -1,
        COLOR_GREEN,   -1,
        COLOR_BLACK,   COLOR_GREEN,
        COLOR_CYAN,    -1,
        COLOR_YELLOW,  -1
    };
}

Theme make_purple_theme() {
    return Theme{
        "purple",
        COLOR_CYAN,    -1,        // border
        COLOR_MAGENTA, -1,        // accent
        COLOR_WHITE,   -1,        // text
        COLOR_BLACK,   COLOR_MAGENTA, // selected
        COLOR_GREEN,   -1,        // completed
        COLOR_YELLOW,  -1         // warning
    };
}

Theme theme_from_name(const str &name){
  if(name == "green"){
    return make_green_theme();
  }
  if(name == "purple"){
    return make_purple_theme();
  }
  return make_default_theme();
}


void apply_theme(const Theme &theme){
  if(!has_colors()){
    return;
  }

  start_color();
  use_default_colors();

  init_pair(PAIR_BORDER,    theme.border_fg,    theme.border_bg);
  init_pair(PAIR_ACCENT,    theme.accent_fg,    theme.accent_bg);
  init_pair(PAIR_TEXT,      theme.text_fg,      theme.text_bg);
  init_pair(PAIR_SELECTED,  theme.selected_fg,  theme.selected_bg);
  init_pair(PAIR_COMPLETED, theme.completed_fg, theme.completed_bg);
  init_pair(PAIR_WARNING,   theme.warning_fg,   theme.warning_bg);
}

