CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -march=native
LDFLAGS = -pthread

TARGET = vertex_cover
SRC = vertex_cover.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
