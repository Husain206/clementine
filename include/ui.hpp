#pragma once
#include "app.hpp"
#include "utils.hpp"

void draw_ui(const App& app);

void draw_header(const App& app);
void draw_list(const App& app);
void draw_footer(const App& app);

str input_dialog(const str& title, const str& prompt);
bool confirm_dialog(const str& msg);

void draw_sidebar(const App& app, int y, int x, int h, int w);
void draw_list_panel(const App& app, int y, int x, int h, int w);
void draw_info_panel(const App& app, int y, int x, int h, int w);

vec<int> filtered_indices(const App& app);

void draw_box(const App& app, int y, int x, int h, int w, bool active);

int menu_dialog(const str& title, const vec<str>& items, int current);
void help_dialog();


// helpers
const char* sort_name(SortMode s);
