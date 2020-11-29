# Parallel Programming

## Exercises Pronunciations

### Exercise 1

Create a parallel program that given an two-dimensional array A of length NxN, checks if the array is strict diagonal dominant. An array is strict diagonal dominant when the type below applies for each row of the array.

    |Aii| > Σ|Aij|  j=0...N-1 i!=j

If the array is strict diagonal dominant then the program will:

- find the max absolute value in the diagonal `m = max(A[i][i]|)`.
    - write 2 different implementitions, one using a reduction clause and one without.

- Using the max value the program found it must calculate a resulting array using the type: 

        if i != j B[i][j] = m - |A[i][j]| else B[i][j] = m

- For the resulting array B the program must find the min value and the position of it

### Exercise 2

Write a multisort function.