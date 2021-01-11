SRC = bst_otroconi.cpp

CXX = c++
CXXFLAGS = -W -Wall -Wextra -g -std=c++14

EXE = $(SRC:.cpp=.x)

.SUFFIXES:
	SUFFIXES =

.SUFFIXES: .cpp .x

all: $(EXE)


.PHONY: all

%.x: %.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(EXE) *~

.PHONY: clean


