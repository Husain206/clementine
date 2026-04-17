#pragma once
#include "app.hpp"
#include "utils.hpp"

void draw_ui(const App& app);

void draw_header(const App& app);
void draw_list(const App& app);
void draw_footer(const App& app);

str input_dialog(const str& title, const str& prompt);
bool confirm_dialog(const str& msg);

