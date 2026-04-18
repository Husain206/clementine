CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -g -Iinclude
LDFLAGS = -lncursesw

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: clementine

clementine: $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) clementine

.PHONY: all clean
