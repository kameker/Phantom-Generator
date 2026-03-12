CXX = g++
CXXFLAGS = -std=c++17 -O2 -I include
LDFLAGS = -static -lgdi32 -o phantom.exe

SRCS = src/main.cpp src/phatom.cpp src/phatom_utils.cpp
TARGET = image_generator.exe

.PHONY: all build run clean

all: build

build:
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS)

run: build
	./$(TARGET)

clean:
	del /q $(TARGET) *.o 2>nul || true
