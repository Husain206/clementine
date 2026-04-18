#include "../include/app.hpp"
#include "../include/storage.hpp"
#include "../include/ui.hpp"
#include <algorithm>

void init_app(App &app, const str &config_path){
  app.config = load_config(config_path);
  app.theme = make_default_theme();
  app.theme.name = app.config.theme_name;

  str theme_path = make_theme_path(app.config.theme_name);
  app.theme = load_theme_file(theme_path, app.theme);  
  
  app.todos = load_todos(app.config.save_file);
  app.next_id = next_todo_id(app.todos);

  app.categories = app.config.categories;
  if(app.categories.empty())
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
  t.category = app.config.default_category;

  app.todos.push_back(t);

  if(std::find(app.categories.begin(), app.categories.end(), t.category) == app.categories.end()){
    app.categories.push_back(t.category);
  }

  autosave(app, "Task added");
}

void toggle_todo(App &app){
  auto idx = filtered_indices(app);
  if(idx.empty()) return;

  int real = app.scroll_offset + app.selected_index;
  if(real >= (int)idx.size()) return;

  Todo& t = app.todos[idx[real]];
  t.done = !t.done;

  autosave(app, t.done ? "task completed" : "task reopened");
}

void delete_todo(App &app){
  auto idx = filtered_indices(app);
  if(idx.empty()) return;

  int real = app.scroll_offset + app.selected_index;
  if(real >= (int)idx.size()) return;

  app.todos.erase(app.todos.begin() + idx[real]);

  if(app.selected_index > 0 && app.selected_index >= (int)filtered_indices(app).size()){
    app.selected_index--;
  }
  // maybe dont save until you quit the app :"D
  autosave(app, "Task deleted");
}


void move_selection_up(App &app){
  if(app.todos.empty()) return;

  app.selected_index--;
  if(app.selected_index < 0){
    app.selected_index = app.todos.size() - 1;
    app.scroll_offset = std::max(0, (int)app.todos.size() - 10);
    return;
  }

  if(app.selected_index < app.scroll_offset){
    app.scroll_offset--;
    if(app.scroll_offset < 0) app.scroll_offset = 0;
  }
}

void move_selection_down(App &app){
  if(app.todos.empty()) return;

  app.selected_index++;
  
  if(app.selected_index >= (int)app.todos.size()){
    app.selected_index = 0;
    app.scroll_offset = 0;
    return;
  }
  int visible_limit = 10;
  if(app.selected_index >= app.scroll_offset + visible_limit){
    app.scroll_offset++;
  }
}

void edit_todo(App &app){
  if(app.todos.empty()) return;

  auto idx = filtered_indices(app);
  if(idx.empty()) return;

  int real = app.scroll_offset + app.selected_index;
  if(real >= (int)idx.size()) return;

  Todo& t = app.todos[idx[real]];
  
  // edit title
  str new_title = input_dialog("Edit task", "Title:");
  if(!new_title.empty()) t.title = new_title;

  // description
  str new_desc = input_dialog("Edit description", "Description:");
  if(!new_desc.empty()) t.description = new_desc;

  // due date
  str new_due = input_dialog("Edit due date", "YYYY-MM-DD:");
  if(!new_due.empty()) t.due_date = new_due;
  
  // pick cateagory
  int current_cat = 0;
  for(int i = 0; i < (int)app.categories.size(); i++){
    if(app.categories[i] == t.category){
      current_cat = i;
      break;
    }
  }
  
  int cat_idx = menu_dialog("category", app.categories, current_cat);
  if(cat_idx >= 0) t.category = app.categories[cat_idx];

  // pick priority

  vec<str> pr = {"LOW", "MED", "HIGH"};
  int p = menu_dialog("Priority", pr, (int)t.priority);
  if(p >= 0) t.priority = (Priority)p;

  autosave(app, "TASK UPDATED");
}

void mark_saved(App& app, const str& msg){
  app.dirty = false;
  app.status_msg = msg;
}

void autosave(App& app, const str& msg){
  save_todos(app.config.save_file, app.todos);
  app.dirty = false;
  app.status_msg = msg;
}

void toggle_logo(App& app){
  app.theme.show_ascii_logo = !app.theme.show_ascii_logo;
  app.status_msg = app.theme.show_ascii_logo ? "Logo enabled" : "Logo disabled";
}
