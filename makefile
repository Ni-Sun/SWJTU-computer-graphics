SRCS := $(wildcard *.cpp)
EXES := $(patsubst %.cpp, %.exe, $(SRCS))

.PHONY: all clean

all: $(EXES)

%.exe: %.cpp
	g++ -o $@ $< -mwindows

clean:
	rm -f *.exe
