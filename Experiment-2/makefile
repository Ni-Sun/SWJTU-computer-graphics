SRCS := $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET = main.exe

$(TARGET): $(OBJS)
	g++ -o $@ $^ -mwindows -lgdi32 -luser32 -lcomdlg32

%.o: %.cpp
	g++ -c $< -mwindows

clean:
	rm -f $(OBJS) $(TARGET)
