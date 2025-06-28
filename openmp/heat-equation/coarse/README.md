## Heat equation solver in parallel with OpenMP

Parallelise our implementation of a two-dimensional heat equation solver using OpenMP.
See previous sections for a general description of the heat equation solver.

Starting point is a working serial code, which you should parallelize
by inserting appropriate OpenMP directives and routines. For
coarse-grain parallelization here, **the whole program should have only
a single parallel region within the main routine**.

  1. Determine the number of threads in the main routine ([cpp/main.cpp](cpp/main.cpp) or [fortran/main.F90](fortran/main.F90))
  2. Parallelize the generation of initial temperature in the routine  `generate_field()` (in [fortran/setup.F90](fortran/setup.F90)) or in the `generate()` method (in [cpp/heat.cpp](cpp/heat.cpp)
  3. Parallelize the main computational routine
     `evolve()` in [cpp/core.cpp](cpp/core.cpp) or [fortran/core.F90](fortran/core.F90).

Note that execution controls might be needed in the main routine and in initialization.

To build the code, please use the provided `Makefile` (by typing `make`)

### Solution

| Threads | Total time (s) | CPU time (cpu-s) | Speedup | Parallel efficiency |
| ------- | -------------- | ---------------- | ------- | ------------------- |
| 1       | 1.363          | 1.363            | 1       | 1                   |
| 4       | 0.701          | 2.804            | 1.94    | 0.4850              |
| 16      | 0.118          | 1.888            | 11.55   | 0.7219              |
| 64      | 0.087          | 5.568            | 15.67   | 0.2448              |
| 128     | 0.062          | 7.936            | 21.98   | 0.1717              |

Idea of coarse grained OpenMP parallelization is to avoid the cost of spinning up threads.
On LUMI, the compiler's OMP implementation seems to recognize reuse of threads in these cases.
This makes fine-grained parallelization perform equally to a coarse-grained parallelization, likely because
span-up threads are not destroyed when a `parallel` OMP block ends and are stashed for reuse later.
This may differ when having fine-grained parallelization with multiple `parallel` blocks throughout the codebase.
