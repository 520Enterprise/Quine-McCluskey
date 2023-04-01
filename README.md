# Quine-McCluskey Algorithm
Quine-McCluskey Algorithm implemented in C++


## Algorithm
1. List all minterms (and don't care terms)
2. Step by step merging one layer at a time (note that only two minimum terms with a distance of 1 can be merged), and pay attention to removing duplicates
3. Find the essential prime implication through the prime implication table

*When merging, there can be optimization (by grouping implication terms by the number of 1 to avoid invalid comparisons)

## Code
GitHub: [Quine-McCluskey](https://github.com/520Enterprise/Quine-McCluskey "Quine-McCluskey")


## Instruction
Divide by space, enter the smallest item expressed in decimal digits in sequence, and enter - 1 after input to indicate completion

After that, the program will automatically give the prime implication term and the prime implication term
