CXX=gcc
CXXFLAGS=-g -Wall -Werror -O2

SRC_FILES = $(filter-out src/configure.c, $(wildcard src/*.c))

all: dogecoin configure

dogecoin: $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lcurl -pthread -lwolfssl -ltidy -lasan -fsanitize=address

configure: src/configure.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f src/*.o

.PHONY: all clean cleantxt
