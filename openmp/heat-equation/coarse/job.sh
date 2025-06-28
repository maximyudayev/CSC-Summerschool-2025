#!/bin/bash
#SBATCH --job-name=heat-omp-coarse
#SBATCH --account=project_462000956
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --output=%x.%J.out
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH --time=00:05:00

srun ./heat_omp.x
#srun ./heat_omp 4000 8000 1000
