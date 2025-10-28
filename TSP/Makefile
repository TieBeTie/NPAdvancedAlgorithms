CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -Wall -Wextra
TARGET = tsp
SOURCE = tsp.cpp

.PHONY: all clean test benchmark

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)
	rm -f results/*.txt
	rm -f perf_results/*.data
	rm -f /tmp/tsp_*.log
	rm -f /tmp/tsp_results.tmp

test: $(TARGET)
	@echo "Running quick test..."
	./$(TARGET) nn_2opt input.txt output.txt

benchmark: $(TARGET)
	@echo "Running benchmark..."
	chmod +x benchmark.sh
	./benchmark.sh all

fast: $(TARGET)
	@echo "Running fast algorithms..."
	chmod +x benchmark.sh
	./benchmark.sh fast

best: $(TARGET)
	@echo "Running best quality algorithms..."
	chmod +x benchmark.sh
	./benchmark.sh best

help:
	@echo "Usage:"
	@echo "  make          - Build the program"
	@echo "  make test     - Run quick test"
	@echo "  make fast     - Benchmark fast algorithms"
	@echo "  make best     - Benchmark best quality algorithms"
	@echo "  make benchmark - Benchmark all algorithms"
	@echo "  make clean    - Clean build and results"
	@echo ""
	@echo "Direct usage:"
	@echo "  ./tsp <algorithm> [input_file] [output_file]"
	@echo ""
	@echo "Available algorithms:"
	@echo "  Basic:"
	@echo "    2approx, 15approx, 15approx_improved, nn"
	@echo "  With 2-opt:"
	@echo "    nn_2opt, multi_nn_2opt, christofides_2opt"
	@echo "  Advanced:"
	@echo "    nn_2opt_3opt, multi_nn_full, christofides_full, ils"