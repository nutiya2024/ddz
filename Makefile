CC = g++
CXXFLAGS = -O2 -fPIC -Wall -std=c++11
LUAINC ?= ../lua
TARGET = YunCheng.so

.PHONY: all clean

all: $(TARGET)

$(TARGET): AutoLock.cpp LuaYunCheng.cpp PermutationCombine.cpp YunChengAI.cpp
	$(CC) $(CXXFLAGS) -shared -o $@ -I$(LUAINC) -lpthread $^

clean:
	rm -f $(TARGET) *.o