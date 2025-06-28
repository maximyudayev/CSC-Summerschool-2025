## Heat equation solver in parallel with OpenMP

Parallelise our implementation of a two-dimensional heat equation solver using OpenMP.
See previous sections for a general description of the heat equation solver.

Starting point is a working serial code, which you should parallelize
by inserting appropriate OpenMP directives and routines. For
fine-grain parallelization here, **parallel regions should be within
subroutines performing computations**.

  1. Determine the number of threads in the main routine ([cpp/main.cpp](cpp/main.cpp) or [fortran/main.F90](fortran/main.F90))
  2. Parallelize the generation of initial temperature in the routine  `generate_field()` (in [fortran/setup.F90](fortran/setup.F90)) or in the `generate()` method (in [cpp/heat.cpp](cpp/heat.cpp)
  3. Parallelize the main computational routine
     `evolve()` in [cpp/core.cpp](cpp/core.cpp) or [fortran/core.F90](fortran/core.F90).

To build the code, please use the provided `Makefile` (by typing `make`).

### Solution

| Threads | Total time (s) | CPU time (cpu-s) | Speedup | Parallel efficiency |
| ------- | -------------- | ---------------- | ------- | ------------------- |
| 1       | 1.363          | 1.363            | 1       | 1                   |
| 4       | 0.703          | 2.812            | 1.94    | 0.4850              |
| 16      | 0.114          | 1.824            | 11.96   | 0.7475              |
| 64      | 0.079          | 5.056            | 17.25   | 0.2695              |
| 128     | 0.053          | 6.784            | 25.72   | 0.2009              |
