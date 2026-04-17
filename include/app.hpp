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

  TodoFilter filter;

  int next_id = 1;

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
