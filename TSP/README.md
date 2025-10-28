# Traveling Salesman Problem

Task: find a Hamiltonian cycle of minimum total length.  
`input.txt` format: list of vertices with coordinates, each line  
`[vertex_id] [x] [y]`.  
`output.txt` format: list of vertices forming a Hamiltonian cycle.  
Goal — tour length < 1,000,000.

## Implemented Algorithms

- **2approx** — MST-based 2-approximation  
- **15approx_improved** — Christofides-style heuristic  
- **nn** — nearest neighbor heuristic  
- **nn_2opt** — nearest neighbor + 2-opt local search  

## Test Results

Dataset size: 33,409 points

| Algorithm | Tour length | Algorithm time (ms) | Total time (ms) | Status |
|-----------|------------|---------------------|----------------|--------|
| 2approx | 1,400,666.66 | 5,811 | 5,835 | > 1,000,000 |
| 15approx_improved | 1,014,242.95 | 22,493 | 22,516 | > 1,000,000 |
| **nn_2opt** | **911,500.19** | 39,754 | 39,793 | **< 1,000,000 — winner** |
| nn | 1,055,329.97 | 3,167 | 3,196 | > 1,000,000 |
