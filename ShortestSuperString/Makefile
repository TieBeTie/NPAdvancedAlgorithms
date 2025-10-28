CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -Wall
TARGET = shortest_superstring
SRC = shortest_superstring.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	rm -rf results/
	rm -f analysis.txt

benchmark: $(TARGET)
	./benchmark.sh

run: $(TARGET)
	./$(TARGET) hybrid input.txt output.txt

.PHONY: all clean benchmark run