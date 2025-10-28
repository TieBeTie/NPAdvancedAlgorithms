# Shortest Superstring

Task: find the shortest possible superstring that contains all input strings as substrings.  
`input.txt` format: list of strings, one per line.  
`output.txt` format: resulting superstring. Goal — length < 26,000.

## Implemented Algorithms

- **lookahead** — greedy overlap with lookahead  
- **bidirectional** — bidirectional greedy merging  
- **hybrid** — combined heuristic (best overlaps + refinement)

## Test Results

| Algorithm | Superstring length | Algorithm time (ms) | Total time (ms) | Status |
|-----------|-------------------|---------------------|----------------|--------|
| lookahead | 27,054 | 32 | 42 | > 26,000 |
| bidirectional | 44,266 | 77 | 88 | > 26,000 |
| **hybrid** | **25,347** | 49 | 58 | **< 26,000 — winner** |
