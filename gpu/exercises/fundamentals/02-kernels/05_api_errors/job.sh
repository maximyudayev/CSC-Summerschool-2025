#!/bin/bash
#SBATCH --job-name=api-errors
#SBATCH --account=project_462000956
#SBATCH --reservation=SummerSchoolGPU
#SBATCH --output=%x.%J.out
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gpus-per-node=1
#SBATCH --time=00:05:00

srun ./main.x
