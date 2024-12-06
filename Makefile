.SUFFIXES:
.SECONDARY:
.PHONY: all clean

LDFLAGS=
LIBS=
C=g++ # Simon : Because of my faulty Ubuntu I need to run "make C=g++-10" or I can't compile, but it should be unchanged for you.
CXXFLAGS=-Wall -Werror -g -std=c++20 -I/Library/Developer/CommandLineTools/usr/include/c++/v1
BIN=BlueHerring
SRCS=main.cpp
OBJS=$(SRCS:.cpp=.o)
DEPS = board_t.hpp file_util.hpp operators_util.hpp piece_t.hpp square_t.hpp

all: $(BIN)

$(BIN): $(OBJS)
	$(C) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp $(DEPS)
	$(C) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS) *~