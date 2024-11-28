.SUFFIXES:
.SECONDARY:
.PHONY: all clean

LDFLAGS=
LIBS=
CXXFLAGS=-Wall -Werror -g -std=c++20 -I/Library/Developer/CommandLineTools/usr/include/c++/v1
BIN=BlueHerring
SRCS=main.cpp
OBJS=$(SRCS:.cpp=.o)

all: $(BIN)

$(BIN): $(OBJS)
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	g++ $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS) *~