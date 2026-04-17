#pragma once

#include "utils.hpp"
#include "models.hpp"

// id|done|priority|category|title|created_at

vec<Todo> load_todos(const str& path);
void save_todos(const str& path, const vec<Todo>& todos);
int next_todo_id(const vec<Todo>& todos);
