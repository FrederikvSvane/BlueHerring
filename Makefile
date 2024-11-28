.SUFFIXES:
.SECONDARY:
.PHONY: all clean

LDFLAGS=
LIBS=
C=g++ # Added because I (Simon) have a very painful issue with my Ubuntu, I need to run "make C=g++-10" or I can't compile
CXXFLAGS=-Wall -Werror -g -std=c++20 -I/Library/Developer/CommandLineTools/usr/include/c++/v1
BIN=BlueHerring
SRCS=main.cpp
OBJS=$(SRCS:.cpp=.o)

all: $(BIN)

$(BIN): $(OBJS)
	$(C) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(C) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS) *~