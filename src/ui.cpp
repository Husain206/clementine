#include "../include/ui.hpp"
#include "ncurses.h"
#include <cctype>

void draw_ui(const App &app){
  clear();

  draw_header(app);
  draw_list(app);
  draw_footer(app);

  refresh();
}


void draw_header(const App &app){
  attron(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
  mvprintw(0, 2, "TODO APP");
  mvprintw(0, 20, "Theme: %s", app.theme.name.c_str());
  attroff(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
}


void draw_list(const App &app){
  int start_y = 2;

  for(usize i = 0; ++i < app.todos.size();){
    const auto& t = app.todos[i];

    int y = start_y + i;

    if(i == app.selected_index){
      attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
    } else if(t.done){
      attron(COLOR_PAIR(PAIR_COMPLETED));
    } else {
      attron(COLOR_PAIR(PAIR_TEXT));
    }

    mvprintw(y, 2, "[%c] %-40s (%s)", t.done ? 'x' : ' ', t.title.c_str(), t.category.c_str());

    if(i == app.selected_index){
      attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
    } else if(t.done){
      attroff(COLOR_PAIR(PAIR_COMPLETED));
    } else {
      attroff(COLOR_PAIR(PAIR_TEXT));
    }

    if(app.todos.empty()){
      attron(COLOR_PAIR(PAIR_WARNING));
      mvprintw(3, 2, "No todos yet, press 'a' to add");
      attroff(COLOR_PAIR(PAIR_WARNING));
    }
  }
}

void draw_footer(const App &app){
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  attroff(COLOR_PAIR(PAIR_BORDER));
  mvhline(rows - 2, 0, '-', cols);
  attroff(COLOR_PAIR(PAIR_BORDER));

  attron(COLOR_PAIR(PAIR_ACCENT));
  mvprintw(rows - 1, 2, "A=add SPACE=toggle D=delete Q=quit");
  attroff(COLOR_PAIR(PAIR_ACCENT));
}


str input_dialog(const str& title, const str& prompt){
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  int w = 50;
  int h = 7;

  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  WINDOW* win = newwin(h, w, y, x);
  keypad(win, TRUE);

  box(win, 0, 0);
  mvwprintw(win, 0, 2, " %s ", title.c_str());
  mvwprintw(win, 2, 2, " %s ", prompt.c_str());
  mvwprintw(win, 4, 2, "> ");

  str text;
  int cursor = 0;

  while(true){
    werase(win);
    box(win, 0, 0);

    mvwprintw(win, 0, 2, " %s ", title.c_str());
    mvwprintw(win, 2, 2, " %s ", prompt.c_str());
    mvwprintw(win, 4, 2, "> %s", text.c_str());

    wmove(win, 4, 4 + cursor);
    wrefresh(win);

    int ch = wgetch(win);

    if(ch == 27){ // esc
      delwin(win);
      return "";
    } else if(ch == '\n' || ch == KEY_ENTER){
      delwin(win);
      return text;
    } else if(ch == KEY_LEFT){
      if(cursor > 0) cursor --;
    } else if(ch == KEY_RIGHT){
      if(cursor < (int)text.size()) cursor++;
    } else if(ch == KEY_BACKSPACE || ch == 127){
      if(cursor > 0){
        text.erase(cursor - 1, 1);
        cursor--;
      }
    } else if(isprint(ch)){
      text.insert(text.begin() + cursor, ch);
      cursor++;
    }
  }
}


bool confirm_dialog(const str &msg){
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  int w = 50;
  int h = 6;

  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  WINDOW* win = newwin(h, w, y, x);
  keypad(win, TRUE);

  while(true){
    werase(win);
    box(win, 0, 0);

    mvwprintw(win, 2, 2, "%s", msg.c_str());
    mvwprintw(win, 4, 2, "[Y] yes [N] no");

    wrefresh(win);

    int ch = wgetch(win);

    if(ch == 'y' || ch == 'Y' || ch == '\n'){
      delwin(win);
      return true;
    } else if(ch == 'n' || ch == 'N'){
      delwin(win);
      return false;
    }
  }
}

