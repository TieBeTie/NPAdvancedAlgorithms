# Vertex Cover

Task: find a vertex cover of minimal size.  
`input.txt` format: list of edges, each line two vertex numbers.  
`output.txt` format: list of vertices in the cover. Goal — < 4,000 vertices.  

## Implemented Algorithms

- **greedy** — greedy (highest-degree vertex)  
- **matching_fast** — matching-based, fast version  
- **matching_post** — matching-based + post-processing  

## Test Results

| Algorithm | Cover size | Algorithm time (ms) | Total time (ms) | Status |
|-----------|------------|-------------------|----------------|--------|
| greedy | 11,752 | 839 | 1,995 | > 4000 |
| matching_fast | 4,050 | 3 | 835 | > 4000 |
| matching_post | 2,050 | 21 | 878 | < 4000 — winner |

