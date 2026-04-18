#include "../include/storage.hpp"
#include <fstream>
#include <string>

static str sanitize_field(str s){
  for(char& c : s){
    if(c == '|') c = '/';
  }
  return s;
}

vec<Todo> load_todos(const str &path){
  vec<Todo> todos;

  std::ifstream in(path);
  if(!in){
    return todos;
  }

  str line;

  while(std::getline(in, line)){
    auto parts = split(line, '|');
    if(parts.size() < 8) continue;

    Todo t;
    t.id = std::stoi(parts[0]);
    t.done = parts[1] == "1";
    t.priority = (Priority)(stoi(parts[2]));
    t.category = parts[3];
    t.title = parts[4];
    t.created_at = parts[5];
    t.description = parts[6];
    t.due_date = parts[7];

    todos.push_back(t);
  }
  return todos;
}

void save_todos(const str &path, const vec<Todo> &todos){
  std::ofstream out(path);

  // for(const auto& t : todos){
  //   out << t.id << "|"
  //       << (t.done ? 1 : 0) << "|"
  //       << (int)t.priority << "|"
  //       << t.category << "|"
  //       << t.title << "|"
  //       << t.created_at << "\n";
  // }

   for(const auto& t : todos){
    out << t.id << '|'
        << (t.done ? 1 : 0) << '|'
        << (int)t.priority << '|'
        << t.category << '|'
        << t.title << '|'
        << t.created_at << '|'
        << sanitize_field(t.description) << '|'
        << t.due_date
        << '\n';
  }
}


int next_todo_id(const vec<Todo> &todos){
  int max_id = 0;
  for(const auto& t : todos){
    if(t.id > max_id){
      max_id = t.id;
    }
  }
  return max_id + 1;
}
