#pragma once
#include "utils.hpp"
#include "ncurses.h"


enum class HeaderStyle {
  Compact,
  Classic,
};

enum class FooterStyle {
  Classic,
  StatusBar,
};

// border_* should define visual structure
// accent_* should stand out for titles and headers
// text_* should be readable for normal content
// selected_* should strongly indicate current focus
// completed_* should be visible but quieter
// warning_* should draw attention
struct Theme {
  str name;

  short bg = -1;
  short text = COLOR_WHITE;
  short dim = COLOR_BLUE;
  short border = COLOR_CYAN;
  short border_active = COLOR_MAGENTA;
  short accent = COLOR_MAGENTA;
  short accent2 = COLOR_CYAN;
  short warning = COLOR_RED;
  short success = COLOR_GREEN;
  short selected_fg = COLOR_BLACK;
  short selected_bg = COLOR_CYAN;

  short panel_title_color = COLOR_YELLOW;
  short dialog_accent_color = COLOR_YELLOW;

  bool heavy_borders = true;
  bool use_shadow = true;
  bool dense_header = true;
  bool all_caps = true;
  bool show_ascii_logo = true;
  bool scanlines = false;
  bool double_borders = false;

  HeaderStyle header_style = HeaderStyle::Classic;
  FooterStyle footer_style = FooterStyle::Classic;

  str header_label = "TASK-NODE";
  str footer_label = "SYS/READY";
};

// ncurses color pair 0 is special/default
enum ColorPairId {
  PAIR_BORDER = 1,
  PAIR_ACCENT,
  PAIR_TEXT,
  PAIR_SELECTED,
  PAIR_COMPLETED,
  PAIR_WARNING,
  PAIR_DIM,
  PAIR_PANEL_TITLE,
  PAIR_DIALOG_ACCENT,
};

Theme make_default_theme();

HeaderStyle header_style_from_name(const str& s);
FooterStyle footer_style_from_name(const str& s);

void apply_theme(const Theme& theme);
short color_from_name(const str& s);

Theme load_theme_file(const str& path, Theme base);
str make_theme_path(const str& theme_name);

