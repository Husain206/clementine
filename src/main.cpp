#include "../include/app.hpp"
#include "../include/ui.hpp"
#include <algorithm>
#include <ncurses.h>

void init_ncurses();
void run(App& app);

int main(void){
  App app;
  init_app(app, "config/clementine.conf");
  init_ncurses();
  run(app);
  
  return 0;
}

/* ------------------ */
void init_ncurses(){
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
}

void run(App& app){
  apply_theme(app.theme);
  bool running = true;

  while(running){
    draw_ui(app);
  
    int ch = getch();

    switch (ch) {
      case 'a':{
        str title = input_dialog("New task", "Title:");
        if (title.empty()) break;

        str desc = input_dialog("Description", "Optional:");
        str due = input_dialog("Due date", "YYYY-MM-DD (optional):");

        int cat_idx = menu_dialog("Category", app.categories, 0);
        if (cat_idx < 0) break;

        vec<str> pr = {"LOW", "MED", "HIGH"};
        int pri_idx = menu_dialog("Priority", pr, 1);
        if (pri_idx < 0) break;

        Todo t;
        t.id = app.next_id++;
        t.title = title;
        t.description = desc;
        t.due_date = due;

        t.category = app.categories[cat_idx];
        t.priority = (Priority)pri_idx;
        t.created_at = now_timestamp("%Y-%m-%d %H:%M");

        app.todos.push_back(t);

        if(std::find(app.categories.begin(), app.categories.end(), t.category) == app.categories.end()){
          app.categories.push_back(t.category);
        }

        autosave(app, "Task added");
      } break;
      case ' ': toggle_todo(app); break;
      case 'd':
        if(confirm_dialog("are you sure you want to delete this task?")) delete_todo(app);
        break;
      case 'q': running = false; break;
      case KEY_UP:
        if (app.focus == Focus::Categories) {
            app.selected_category =
                (app.selected_category - 1 + app.categories.size())
                % app.categories.size();
            app.selected_index = 0;
        } else {
            move_selection_up(app);
        }
        break;

      case KEY_DOWN:
        if (app.focus == Focus::Categories) {
            app.selected_category =
                (app.selected_category + 1)
                % app.categories.size();
            app.selected_index = 0;
        } else {
            move_selection_down(app);
        }
        break;
      case '\t': app.focus = (app.focus == Focus::Todos) ? Focus::Categories : Focus::Todos; break;

      case 'e': edit_todo(app); break;

      case '/':{
        str q = input_dialog("search", "Query");
        app.search_query = q;
        app.selected_index = 0;
        app.scroll_offset = 0;
        break;    
      }

      case 'C':
      case 'c':
        app.search_query.clear();
        app.selected_index = 0;
        app.scroll_offset = 0;
        app.status_msg = "search cleared";
        break;

      case '?':
        help_dialog();
        app.status_msg = "Help closed";
        break;

      case '1':
        app.quick_filters = QuickFilters::All;
        app.selected_index = 0;
        app.scroll_offset = 0;
        app.status_msg = "Filter: all";
        break;
      
      case '2':
        app.quick_filters = QuickFilters::OpenOnly;
        app.selected_index = 0;
        app.scroll_offset = 0;
        app.status_msg = "Filter: open";
        break;
      
      case '3':
        app.quick_filters = QuickFilters::DoneOnly;
        app.selected_index = 0;
        app.scroll_offset = 0;
        app.status_msg = "Filter: done";
        break;
      
      case '4':
        app.quick_filters = QuickFilters::HighPriority;
        app.selected_index = 0;
        app.scroll_offset = 0;
        app.status_msg = "Filter: high priority";
        break;

      case 's':
      case 'S':
        if(app.sort_mode == SortMode::Created) app.sort_mode = SortMode::Priority;
        else if(app.sort_mode == SortMode::Priority) app.sort_mode = SortMode::DueDate;
        else app.sort_mode = SortMode::Created;

        app.scroll_offset = 0;
        app.selected_index = 0;
        app.status_msg = str("Sort: ") + sort_name(app.sort_mode);
        break;

      case 'l':
      case 'L':
      toggle_logo(app);
      break;
    }
  }
  endwin();
  save_app(app);
}
