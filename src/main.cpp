#include "../include/app.hpp"
#include "../include/ui.hpp"
#include <ncurses.h>


int main(void){
  App app;
  init_app(app, "config/clementine");

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  apply_theme(app.theme);
  bool running = true;

  while(running){
    draw_ui(app);
  
    int ch = getch();

    switch (ch) {
      case 'a':{
        str text = input_dialog("new task", "enter task");
        if(!text.empty()) add_todo(app, text);
      } break;
      case ' ': toggle_todo(app); break;
      case 'd':
        if(confirm_dialog("are you sure you want to delete this task?")) delete_todo(app);
        break;
      case 'q': running = false; break;
      case KEY_UP: move_selection_up(app); break;
      case KEY_DOWN: move_selection_down(app); break;
    }
  }
  endwin();
  save_app(app);
  
  return 0;
}
