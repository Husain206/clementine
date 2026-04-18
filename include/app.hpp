#pragma once

#include "utils.hpp"
#include "models.hpp"
#include "config.hpp"
#include "theme.hpp"

enum class Focus {
  Categories,
  Todos,
};

struct App {
  vec<Todo> todos;
  int selected_index = 0;

  vec<str> categories = {"all"};
  int selected_category = 0;

  Focus focus = Focus::Todos;
  
  QuickFilters quick_filters = QuickFilters::All;
  SortMode sort_mode = SortMode::Created;

  std::string search_query;

  str status_msg = "READY";
  bool dirty = false;
  
  int next_id = 1;

  int scroll_offset = 0;

  Config config;
  Theme theme;
};

void init_app(App& app, const str& config_path);
void save_app(const App& app);

void add_todo(App& app, const str& title);
void toggle_todo(App& app);
void delete_todo(App& app);

void move_selection_up(App& app);
void move_selection_down(App& app);

void edit_todo(App& app);

void mark_saved(App& app, const str& msg = "saved");
void autosave(App& app, const str& msg);

void toggle_logo(App& app);
