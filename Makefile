CXX = g++
CXXFLAGS = -std=c++17 -O2 -I include
LDFLAGS_WIN = -static -lgdi32 -o phantom.exe
LDFLAGS_LINUX = -static -lgdi32 -o phantom

SRCS = src/main.cpp src/phatom.cpp src/phatom_utils.cpp
TARGET = image_generator.exe

.PHONY: all build buildlinux run clean

all: build

build:
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS_WIN)

buildlinux:
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS_LINUX)

run: build
	./$(TARGET)

clean:
	del /q $(TARGET) *.o 2>nul || true
