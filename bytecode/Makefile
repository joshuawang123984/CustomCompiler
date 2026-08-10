CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,src/,*.cpp)
OBJ = $(SRC:.cpp=.o)

compiler: $(OBJ)
	$(CXX) $(OBJ) -o compiler

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) compiler