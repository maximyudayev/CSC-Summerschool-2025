#!/bin/bash
#SBATCH --job-name=pi
#SBATCH --account=project_462000956
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --output=%x.%J.out
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --time=00:01:00

srun ./pi
