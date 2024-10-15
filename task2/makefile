# 获取当前目录下所有的 cpp 文件
SRCS := $(wildcard *.cpp)
# 将 cpp 文件替换为对应的可执行文件名
EXES := $(patsubst %.cpp,%,$(SRCS))

# 默认目标，编译所有的可执行文件
all: $(EXES)

# 编译每个 cpp 文件为对应的可执行文件
%: %.cpp
	g++ -o $@ $< -mwindows

# 清理生成的可执行文件
clean:
	rm -f $(EXES)
