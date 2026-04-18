#include "../include/ui.hpp"
#include "ncurses.h"
#include <algorithm>
#include <cctype>

static const char* priority_str(Priority p){
  switch (p) {
    case Priority::Low: return "LOW";
    case Priority::High: return "HIGH";
    default: return "MED";
  }
}

static str make_filename(const Todo& t){
  str s = t.title;
  for(char& c : s){
    if(c == ' ') c = '_';
  }
  if(s.size() > 16) s = s.substr(0, 16);
  return s + (t.done ? ".done" : ".tsk");
}

static str fake_size(Priority p){
  switch (p) {
    case Priority::Low: return "12k";
    case Priority::High: return "69k";
      default: return "42k";
  }
}

static str today_date(){
  return now_timestamp("%Y-%m-%d");
}

static bool is_overdue(const Todo& t){
  return !t.done && !t.due_date.empty() && t.due_date < today_date();
}

static bool passes_quick_filter(const App& app, const Todo& t){
  switch (app.quick_filters) {
    case QuickFilters::OpenOnly: return !t.done;
    case QuickFilters::DoneOnly: return t.done;
    case QuickFilters::HighPriority: return t.priority == Priority::High;
      default: return true;
  }
}

static const char* filter_name(QuickFilters f){
  switch (f) {
    case QuickFilters::OpenOnly: return "OPEN";
    case QuickFilters::DoneOnly: return "DONE";
    case QuickFilters::HighPriority: return "HIGH";
      default: return "ALL";
  }
}

const char* sort_name(SortMode s){
  switch (s) {
    case SortMode::Priority: return "PRIORITY";
    case SortMode::DueDate: return "DUE";
      default: return "CREATED";
  }
}

void draw_ui(const App &app){
  clear();

  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  if(app.theme.scanlines){
   attron(COLOR_PAIR(PAIR_DIM));
   for(int y = 0; y < rows; y += 2)
     mvhline(y, 0, ' ', cols);
   attroff(COLOR_PAIR(PAIR_DIM));
  }

  int sidebar_w = app.config.sidebar_width;
  int info_w = app.config.info_width;
  int list_w = cols - sidebar_w - info_w;

  int top = 3;
  int height = rows - 6;
  
  draw_header(app);
  
  draw_sidebar(app, top, 0, height, sidebar_w);
  draw_list_panel(app, top, sidebar_w, height, list_w);
  draw_info_panel(app, top, sidebar_w + list_w, height, info_w);
  
  draw_footer(app);

  refresh();
}


void draw_header(const App &app){
  int cols = getmaxx(stdscr);

  if(app.theme.header_style == HeaderStyle::Compact){
    attron(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
    mvprintw(0, 2, "ACIDTODO :: CYBERPANEL");
    mvprintw(0, cols - 22, "WHATS > TODO/QUEUE");
    attroff(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);

    attron(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
    mvprintw(1, 2, "ON %s", now_timestamp("%m/%d/%y %I:%M%p").c_str());
    mvprintw(1, cols/2 - 6, " %s ", app.theme.header_label.c_str());
    mvprintw(1, cols - 22, "THEME:%s", app.theme.name.c_str());
    attroff(COLOR_PAIR(PAIR_BORDER) | A_BOLD);

    attron(COLOR_PAIR(PAIR_ACCENT));
    mvhline(2, 0, ACS_HLINE, cols);
    attroff(COLOR_PAIR(PAIR_ACCENT));
    return;
  }

  attron(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
  mvprintw(0, 2, "ACIDTODO");
  mvprintw(0, 12, "::");
  mvprintw(0, 15, "CYBERPANEL");
  mvprintw(0, cols - 26, "ON %s", now_timestamp("%m/%d/%y %I:%M%p").c_str());
  attroff(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);

  attron(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
  mvprintw(1, 2, "WHATS");
  mvprintw(1, 9, ">");
  mvprintw(1, 11, "TODO/CONFERENCE/QUEUE");
  mvprintw(1, cols - 24, "THEME:%s", app.theme.name.c_str());
  attroff(COLOR_PAIR(PAIR_BORDER) | A_BOLD);

  attron(COLOR_PAIR(PAIR_ACCENT));
  mvhline(2, 0, ACS_HLINE, cols);
  attroff(COLOR_PAIR(PAIR_ACCENT));
}

void draw_list(const App &app){
  int start_y = 2;

    for (size_t i = 0; i < app.todos.size(); ++i){
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
    }

    if(app.todos.empty()){
      attron(COLOR_PAIR(PAIR_WARNING));
      mvprintw(3, 2, "No todos yet, press 'a' to add");
      attroff(COLOR_PAIR(PAIR_WARNING));
    }
}

void draw_footer(const App &app){
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  if(app.theme.footer_style == FooterStyle::StatusBar){
    attron(COLOR_PAIR(PAIR_ACCENT));
    mvhline(rows - 2, 0, ' ', cols);
    mvprintw(rows - 2, 1, "A/ADD E/EDIT SPC/TOGGLE D/DEL S/SORT / SEARCH C/CLEAR Q/QUIT");
    attroff(COLOR_PAIR(PAIR_ACCENT));

    attron(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
    mvhline(rows - 1, 0, ' ', cols);
    mvprintw(rows - 1, 1, "%s :: %s", app.theme.footer_label.c_str(), app.status_msg.c_str());
    attroff(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
    return;
  }

  attron(COLOR_PAIR(PAIR_ACCENT));
  mvhline(rows - 3, 0, ACS_HLINE, cols);
  attroff(COLOR_PAIR(PAIR_ACCENT));

  attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
  mvhline(rows - 2, 0, ' ', cols);
  mvprintw(rows - 2, 1, "1/ALL  2/OPEN  3/DONE  4/HIGH  S/SORT  //SEARCH  C/CLEAR  TAB/SWITCH  A/ADD  ?/HELP  Q/QUIT");
  attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);

  attron(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
  mvhline(rows - 1, 0, ' ', cols);
  mvprintw(rows - 1, 1, "OPEN:%ld  DONE:%ld  CAT:%s  FOCUS:%s  DB:%s",
           std::count_if(app.todos.begin(), app.todos.end(), [](const Todo& t){ return !t.done; }),
           std::count_if(app.todos.begin(), app.todos.end(), [](const Todo& t){ return t.done; }),
           app.categories[app.selected_category].c_str(),
           app.focus == Focus::Categories ? "UPLOADER" : "TASKS",
           app.config.save_file.c_str());
  attroff(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
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

    wattron(win, COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);
    mvwprintw(win, 0, 2, " INPUT NODE ");
    wattroff(win, COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);

    wattron(win, COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);
    mvwprintw(win, 0, w - 15, " %s ", title.c_str());
    wattroff(win, COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);
    
    wattron(win, COLOR_PAIR(PAIR_BORDER));
    mvwprintw(win, 2, 2, "%s", prompt.c_str());
    mvwprintw(win, h - 2, 2, "ENTER=OK  ESC=ABORT");
    wattroff(win, COLOR_PAIR(PAIR_BORDER));
    
    wattron(win, COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
    mvwprintw(win, 4, 2, "> %-*s", w - 6, text.c_str());
    wattroff(win, COLOR_PAIR(PAIR_SELECTED) | A_BOLD);

    // wattron(win, COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
    // mvwprintw(win, 0, 2, " SYSOP CONFIRM ");
    // wattroff(win, COLOR_PAIR(PAIR_ACCENT) | A_BOLD);

    // wattron(win, COLOR_PAIR(PAIR_BORDER));
    // mvwprintw(win, 2, 2, "%s", prompt.c_str());
    // mvwprintw(win, 4, 2, "[Y] ACCEPT   [N] ABORT");
    // wattroff(win, COLOR_PAIR(PAIR_BORDER));

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

    wattron(win, COLOR_PAIR(PAIR_DIALOG_ACCENT) | A_BOLD);
    // wattron(win, COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
    mvwprintw(win, 2, 2, "%s", msg.c_str());
    wattroff(win, COLOR_PAIR(PAIR_DIALOG_ACCENT) | A_BOLD);
    wattron(win, COLOR_PAIR(PAIR_BORDER));
    mvwprintw(win, 4, 2, "[Y] yes [N] no");
    wattroff(win, COLOR_PAIR(PAIR_BORDER));

    // mvwprintw(win, 2, 2, "%s", msg.c_str());
    // mvwprintw(win, 4, 2, "[Y] yes [N] no");
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

void draw_sidebar(const App& app, int y, int x, int h, int w) {
    draw_box(app, y, x, h, w, app.focus == Focus::Categories);

    attron(COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);
    mvprintw(y, x + 2, " UPLOADERS ");
    attroff(COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);

    for (int i = 0; i < (int)app.categories.size(); ++i) {
        int yy = y + 2 + i;

        if (app.focus == Focus::Categories && i == app.selected_category) {
            attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
        } else {
            attron(COLOR_PAIR(PAIR_ACCENT));
        }

        mvprintw(yy, x + 1, "%-*s", w-2, app.categories[i].c_str());

        if (app.focus == Focus::Categories && i == app.selected_category) {
            attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
        } else {
            attroff(COLOR_PAIR(PAIR_ACCENT));
        }
    }
}

vec<int> filtered_indices(const App& app) {
    vec<int> out;
    str cat = app.categories[app.selected_category];
    str q = to_lower(app.search_query);

    for (int i = 0; i < (int)app.todos.size(); ++i) {
        const auto& t = app.todos[i];

        bool category_ok = (cat == "ALL" || t.category == cat);
        bool search_ok = app.search_query.empty() ||
                         to_lower(t.title).find(q) != str::npos ||
                         to_lower(t.description).find(q) != str::npos;
        bool quick_ok = passes_quick_filter(app, t);

        if (category_ok && search_ok && quick_ok) {
            out.push_back(i);
        }
    }

    std::sort(out.begin(), out.end(), [&](int a, int b){
        const Todo& A = app.todos[a];
        const Todo& B = app.todos[b];

        switch(app.sort_mode){
          case SortMode::Priority:
            if ((int)A.priority != (int)B.priority)
              return (int)A.priority > (int)B.priority;
            return A.created_at > B.created_at;

          case SortMode::DueDate:
            if (A.due_date.empty() != B.due_date.empty())
              return !A.due_date.empty();
            if (A.due_date != B.due_date)
              return A.due_date < B.due_date;
            return A.created_at > B.created_at;

          case SortMode::Created:
          default:
            return A.created_at > B.created_at;
        }
    });

    return out;
}

void draw_list_panel(const App& app, int y, int x, int h, int w) {
    draw_box(app, y, x, h, w, app.focus == Focus::Todos);

    // attron(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
    // mvprintw(y + 1, x + 2, "%-18s %-5s %-10s %-10s %-4s", "NAME", "PRI", "AREA", "DUE", "ST");
    // attroff(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
    mvprintw(y, x, "F");

    attron(COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);
    mvprintw(y, x + 2, " filename ");
    // mvprintw(y, x + 24, " size ");
    mvprintw(y, x + 33, " conference ");
    mvprintw(y, x + w - 24, " date ");
    mvprintw(y, x + w - 14, " time ");
    // attroff(COLOR_PAIR(PAIR_BORDER) | A_BOLD);
    attroff(COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);

    auto idx = filtered_indices(app);
    int visible = h - 3;
    str q = to_lower(app.search_query);

    for (int i = 0; i < visible; ++i) {
        int real = app.scroll_offset + i;
        if (real >= (int)idx.size()) break;

        int yy = y + 2 + i;
        const auto& t = app.todos[idx[real]];
        bool selected = (app.focus == Focus::Todos && real == app.selected_index);
        bool matched = !q.empty() && (
            to_lower(t.title).find(q) != str::npos ||
            to_lower(t.description).find(q) != str::npos
        );

        if (selected) {
            attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
            mvprintw(yy, x + 1, "%-*s", w - 2, "");
            attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
        }

        if (selected) attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
        else if (t.done) attron(COLOR_PAIR(PAIR_COMPLETED));
        else attron(COLOR_PAIR(PAIR_TEXT));

        // mvprintw(yy, x + 2, "%-18.18s", make_filename(t).c_str());
        // mvprintw(yy, x + 21, "%-5s", priority_str(t.priority));
        // mvprintw(yy, x + 27, "%-10.10s", t.category.c_str());

        mvprintw(yy, x + 2, "%-18.18s", make_filename(t).c_str());
        // mvprintw(yy, x + 24, "%-7s", fake_size(t.priority).c_str());
        mvprintw(yy, x + 33, "%-12.12s", t.category.c_str());
        mvprintw(yy, x + w - 24, "%-8s", now_timestamp("%m/%d/%y").c_str());
        mvprintw(yy, x + w - 14, "%-8s", now_timestamp("%I:%M%p").c_str());

        if (selected) attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
        else if (t.done) attroff(COLOR_PAIR(PAIR_COMPLETED));
        else attroff(COLOR_PAIR(PAIR_TEXT));

        if (is_overdue(t)) {
            attron(COLOR_PAIR(PAIR_WARNING) | A_BOLD);
            mvprintw(yy, x + 38, "%-10.10s", t.due_date.empty() ? "----" : t.due_date.c_str());
            attroff(COLOR_PAIR(PAIR_WARNING) | A_BOLD);
        } else {
            if (selected) attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
            else attron(COLOR_PAIR(PAIR_BORDER));
            mvprintw(yy, x + 38, "%-10.10s", t.due_date.empty() ? "----" : t.due_date.c_str());
            if (selected) attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
            else attroff(COLOR_PAIR(PAIR_BORDER));
        }

        if (matched) {
            attron(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
            mvprintw(yy, x + w - 5, "FND");
            attroff(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
        } else {
            if (selected) attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
            else attron(COLOR_PAIR(PAIR_BORDER));
            mvprintw(yy, x + w - 5, "%-4s", t.done ? "OK" : "...");
            if (selected) attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
            else attroff(COLOR_PAIR(PAIR_BORDER));
        }
    }

    if (idx.empty()) {
        attron(COLOR_PAIR(PAIR_WARNING) | A_BOLD);
        mvprintw(y + 4, x + 3, "NO FILES MATCH CURRENT FILTER");
        attroff(COLOR_PAIR(PAIR_WARNING) | A_BOLD);
    }
}

void draw_info_panel(const App& app, int y, int x, int h, int w) {
    draw_box(app, y, x, h, w, false);

    auto idx = filtered_indices(app);
    if (idx.empty()) return;

    int real = app.scroll_offset + app.selected_index;
    if (real >= (int)idx.size()) return;

    const auto& t = app.todos[idx[real]];

    attron(COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);
    // attron(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
    mvprintw(y, x, "L]");
    mvprintw(y, x + 3, " CONTROL ");
    attroff(COLOR_PAIR(PAIR_PANEL_TITLE) | A_BOLD);
    // attroff(COLOR_PAIR(PAIR_ACCENT) | A_BOLD);

    attron(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
    mvprintw(y + 1, x + 2, "ID:%03d", t.id);
    mvprintw(y + 2, x + 2, "STATE:%s", t.done ? "DONE" : "OPEN");
    mvprintw(y + 3, x + 2, "PRI:%s", priority_str(t.priority));
    mvprintw(y + 4, x + 2, "AREA:%s", t.category.c_str());
    attroff(COLOR_PAIR(PAIR_SELECTED) | A_BOLD);

    attron(COLOR_PAIR(PAIR_BORDER));
    mvprintw(y + 6, x + 2, "TITLE:");
    mvprintw(y + 7, x + 2, "%-*.*s", w - 4, w - 4, t.title.c_str());
    mvprintw(y + 9, x + 2, "FILE:%s", make_filename(t).c_str());
    // mvprintw(y + 11, x + 2, "SIZE:%s", fake_size(t.priority).c_str());
    mvprintw(y + 13, x + 2, "DATE:%s", now_timestamp("%m/%d/%y").c_str());
    attroff(COLOR_PAIR(PAIR_BORDER));

    attron(COLOR_PAIR(PAIR_ACCENT));
    mvprintw(y + h - 2, x + 2, "[ADD] [EDIT] [TOGGLE] [KILL]");
    attroff(COLOR_PAIR(PAIR_ACCENT));
}

void draw_box(const App& app, int y, int x, int h, int w, bool active) {
    int pair = active ? PAIR_ACCENT : PAIR_BORDER;
    attron(COLOR_PAIR(pair) | A_BOLD);

    if(app.theme.double_borders){
      mvaddstr(y, x, "╔");
      mvaddstr(y, x + w - 1, "╗");
      mvaddstr(y + h - 1, x, "╚");
      mvaddstr(y + h - 1, x + w - 1, "╝");

      for(int i = x + 1; i < x + w - 1; ++i){
        mvaddstr(y, i, "═");
        mvaddstr(y + h - 1, i, "═");
      }
      for(int i = y + 1; i < y + h - 1; ++i){
        mvaddstr(i, x, "║");
        mvaddstr(i, x + w - 1, "║");
      }
    } else {
      chtype hch = app.theme.heavy_borders ? ACS_HLINE : '-';
      chtype vch = app.theme.heavy_borders ? ACS_VLINE : '|';
      chtype ul  = app.theme.heavy_borders ? ACS_ULCORNER : '+';
      chtype ur  = app.theme.heavy_borders ? ACS_URCORNER : '+';
      chtype ll  = app.theme.heavy_borders ? ACS_LLCORNER : '+';
      chtype lr  = app.theme.heavy_borders ? ACS_LRCORNER : '+';

      mvaddch(y, x, ul);
      mvaddch(y, x + w - 1, ur);
      mvaddch(y + h - 1, x, ll);
      mvaddch(y + h - 1, x + w - 1, lr);

      for (int i = x + 1; i < x + w - 1; ++i) {
        mvaddch(y, i, hch);
        mvaddch(y + h - 1, i, hch);
      }

      for (int i = y + 1; i < y + h - 1; ++i) {
        mvaddch(i, x, vch);
        mvaddch(i, x + w - 1, vch);
      }
    }

    attroff(COLOR_PAIR(pair) | A_BOLD);

    if(app.theme.use_shadow){
      attron(COLOR_PAIR(PAIR_BORDER));
      for(int i = x + 1; i < x + w; ++i){
        if(y + h < getmaxy(stdscr)) mvaddch(y + h, i, '.');
      }
      for(int i = y + 1; i < y + h; ++i){
        if(x + w < getmaxx(stdscr)) mvaddch(i, x + w, '.');
      }
      attroff(COLOR_PAIR(PAIR_BORDER));
    }
}

int menu_dialog(const str& title, const vec<str>& items, int current){
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  int w = 35;
  int h = std::max(6, (int)items.size() + 4);

  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  WINDOW* win = newwin(h, w, y, x);
  keypad(win, TRUE);

  int sel = current;

  while(true){
    werase(win);
    box(win, 0, 0);

    wattron(win, COLOR_PAIR(PAIR_DIALOG_ACCENT) | A_BOLD);
    // wattron(win, COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
    mvwprintw(win, 0, 2, " %s ", title.c_str());
    wattroff(win, COLOR_PAIR(PAIR_DIALOG_ACCENT) | A_BOLD);
    
    for (size_t i = 0; i < items.size(); ++i){
      if((int)i == sel)
        wattron(win, COLOR_PAIR(PAIR_SELECTED));
      else 
        wattron(win, COLOR_PAIR(PAIR_TEXT));
      
      mvwprintw(win, 2 + (int)i, 2, "%-28s", items[i].c_str());

      if ((int)i == sel) 
          wattroff(win, COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
      else 
          wattroff(win, COLOR_PAIR(PAIR_TEXT));
      }

      wattron(win, COLOR_PAIR(PAIR_BORDER));
      mvwprintw(win, h - 2, 2, "UP/DOWN | ENTER OK | ESC CANCEL");
      wattroff(win, COLOR_PAIR(PAIR_BORDER));
      wrefresh(win);

      int ch = wgetch(win);

      switch (ch) {
        case KEY_UP: sel = (sel - 1 + (int)items.size()) % (int)items.size(); break;
        case KEY_DOWN: sel = (sel + 1) % (int)items.size(); break;
        case '\n': case KEY_ENTER: delwin(win); return sel;
        case 27: delwin(win); return -1;
      } 
    }
  }

void help_dialog(){
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  int w = 64;
  int h = 14;
  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  WINDOW* win = newwin(h, w, y, x);
  keypad(win, TRUE);

  werase(win);
  box(win, 0, 0);

  wattron(win, COLOR_PAIR(PAIR_ACCENT) | A_BOLD);
  mvwprintw(win, 0, 2, " Help ");
  wattroff(win, COLOR_PAIR(PAIR_ACCENT) | A_BOLD);

  wattron(win, COLOR_PAIR(PAIR_TEXT));
  mvwprintw(win, 2, 2, "TAB        switch focus");
  mvwprintw(win, 3, 2, "UP/DOWN    move selection");
  mvwprintw(win, 4, 2, "A          add task");
  mvwprintw(win, 5, 2, "E          edit task");
  mvwprintw(win, 6, 2, "SPACE      toggle done");
  mvwprintw(win, 7, 2, "D          delete task");
  mvwprintw(win, 8, 2, "/          search");
  mvwprintw(win, 9, 2, "C          clear search");
  mvwprintw(win, 11, 2, "Q          quit");
  wattroff(win, COLOR_PAIR(PAIR_TEXT));

  wattron(win, COLOR_PAIR(PAIR_WARNING));
  mvwprintw(win, 12, 2, "Press any key to close");
  wattroff(win, COLOR_PAIR(PAIR_WARNING));

  wrefresh(win);
  wgetch(win);
  delwin(win);
}
