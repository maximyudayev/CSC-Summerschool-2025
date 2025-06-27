#!/bin/bash
#SBATCH --job-name=pi
#SBATCH --account=project_462000956
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --output=%x.%J.out
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:05:00

srun ./heat_mpi
#srun ./heat_mpi 4000 8000 1000
