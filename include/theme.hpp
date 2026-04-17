#pragma once
#include "utils.hpp"

// border_* should define visual structure
// accent_* should stand out for titles and headers
// text_* should be readable for normal content
// selected_* should strongly indicate current focus
// completed_* should be visible but quieter
// warning_* should draw attention
struct Theme {
  str name;
  short border_fg = 0;  
  short border_bg = -1;
  short accent_fg = 0;
  short accent_bg = -1;
  short text_fg = 0;
  short text_bg = -1;
  short selected_fg = 0;
  short selected_bg = -1;
  short completed_fg = 0;
  short completed_bg = -1;
  short warning_fg = 0;
  short warning_bg = -1;
};

// ncurses color pair 0 is special/default
enum ColorPairId {
  PAIR_BORDER = 1,
  PAIR_ACCENT,
  PAIR_TEXT,
  PAIR_SELECTED,
  PAIR_COMPLETED,
  PAIR_WARNING,
};

Theme make_default_theme();
Theme make_green_theme();
Theme make_purple_theme();
Theme theme_from_name(const str& name);
void apply_theme(const Theme& theme);
