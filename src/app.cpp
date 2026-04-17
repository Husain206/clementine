#include "../include/app.hpp"
#include "../include/storage.hpp"
#include <algorithm>

void init_app(App &app, const str &config_path){
  app.config = load_config(config_path);
  app.theme = theme_from_name(app.config.theme_name);
  
  app.todos = load_todos(app.config.save_file);
  app.next_id = next_todo_id(app.todos);

  app.filter.show_completed = app.config.show_completed;
  app.filter.show_archived = app.config.show_archived;

  app.categories = {"all"};
  for(const auto& t : app.todos){
    if(std::find(app.categories.begin(), app.categories.end(), t.category) == app.categories.end()){
      app.categories.push_back(t.category);
    }
  }
}

void save_app(const App &app){
  save_todos(app.config.save_file, app.todos);
}

void add_todo(App &app, const str &title){
  Todo t;
  t.id = app.next_id++;
  t.title = title;
  t.category = app.config.default_catagory;
  t.created_at = now_timestamp(app.config.date_format);

  app.todos.push_back(t);
}

void toggle_todo(App &app){
  if(app.todos.empty()) return;

  app.todos[app.selected_index].done = !app.todos[app.selected_index].done;
}

void delete_todo(App &app){
  if(app.todos.empty()) return;

  app.todos.erase(app.todos.begin() + app.selected_index);

  if(app.selected_index >= (int)app.todos.size()){
    app.selected_index = MAX(0, (int)app.todos.size() - 1);
  }
}


void move_selection_up(App &app){
  if(app.todos.empty()) return;

  app.selected_index--;
  if(app.selected_index < 0){
    app.selected_index = app.todos.size() - 1;
  }
}

void move_selection_down(App &app){
  if(app.todos.empty()) return;

  app.selected_index++;
  if(app.selected_index >= app.todos.size()){
    app.selected_index = 0;
  }
}
