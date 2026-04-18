#pragma once
#include "utils.hpp"

enum class Priority {
  Low,
  Medium,
  High,
};

enum class QuickFilters {
  All,
  OpenOnly,
  DoneOnly,
  HighPriority,
};

enum class SortMode {
  Created,
  Priority,
  DueDate,
};

struct Todo {
  int id = 0;
  str title;
  str description;
  str category = "general";
  bool done = false;
  bool archived = false;
  Priority priority = Priority::Medium;
  str created_at;
  str updated_at;
  str due_date;
};
