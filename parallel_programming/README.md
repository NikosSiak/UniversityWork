# Parallel Programming

## Exercises Pronunciations

### Exercise 1

Create a parallel program that given an array X of length n, it finds the average value of the array, the dispersion and an array D which is calculated by the formula:

`Di  = ((Xi - min(X)) / (max(X) - min(X))) * 100`

The program must run until the user selects to exit.

### Exercise 2

Create a parallel program that given the following arrays:

- A a one dimension array of length n
- B a one dimension array of length n
- C a two dimension array of n rows and n columns
- D a two dimension array of n rows and n columns

Calculates the following operations:

1. C + D
1. C * B
1. A * B
1. C * D
1. Max(B)

The program must run until the user selects to exit.

### Exercise 3

Create a parallel [Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) that does the following:

1. Ask the user for the size of the world
1. Ask the user for the number of cycles to emulate

For each cycle print the following data:

1. The number of alive cells
1. The number of cells that died from lonely
1. The number of cells died from overpopulation
1. The number of cells borned
