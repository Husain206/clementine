#include "../include/utils.hpp"
#include <cctype>
#include <ctime>

str trim(const str &s){
  size_t start = 0;
  while(start < s.size() && isspace((unsigned char)s[start])) start++;
  size_t end = s.size();
  while(end > start && isspace((unsigned char)s[end - 1])) end--;

  return s.substr(start, end - start);
}

str to_lower(str s){
  for(auto& ch : s){
    ch = (char)(std::tolower((unsigned char)ch));
  }
  return s;
}

str to_upper(str s){
  for(auto& ch : s){
    ch = (char)(std::toupper((unsigned char)ch));
  }
  return s;
  
}

vec<str> split(const str &s, char delimiter){
  vec<str> parts;
  str current;

  for(auto ch : s){
    if(ch == delimiter){
      parts.push_back(current);
      current.clear();
    } else {
      current.push_back(ch);
    }
  }
  parts.push_back(current);
  return parts;
}


bool parse_bool(const str &value){
  const str normalized = to_lower(trim(value));

  return normalized == "1" ||
         normalized == "true" ||
         normalized == "yes" ||
         normalized == "on";
}

str now_timestamp(const str& format){
  time_t now = time(nullptr);
  tm local_tm{};

#if defined (_WIN32)
  localtime_s(&local_tm, &now);
#else
  local_tm = *localtime(&now);
#endif

  char buffer[128];
  if(strftime(buffer, sizeof(buffer), format.c_str(), &local_tm) == 0){
    return "";
  }
  
  return str(buffer);
}
