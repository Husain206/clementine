#pragma once

#include <string>
#include <vector>
#include <cstdint>

/* unsinged */
using u8    = uint8_t; 
using u16   = uint16_t; 
using u32   = uint32_t; 
using u64   = uint64_t;
using uint  = unsigned int; 

using usize = size_t;
using isize = ptrdiff_t;

/* singed */
using i8    = int8_t;
using i16   = int16_t; 
using i32   = int32_t;
using i64   = int64_t; 

/* float */
using f64   = double;
using f32   = float;


/* string */
using strview = std::string_view;
using str = std::string;

/* vector */
template<typename T>
using vec = std::vector<T>;

/* array helpers */
#define ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))


/* MIN MAX */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

str trim(const str& s);
str to_lower(str s);
str to_upper(str s);
vec<str> split(const str& s, char delimiter);
// handles "true", "1", "yes", etc
bool parse_bool(const str& value);
str now_timestamp(const str& format = "%Y-%m-%d %H:%M");
