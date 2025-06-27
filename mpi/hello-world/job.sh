#!/bin/bash
#SBATCH --job-name=hello
#SBATCH --account=project_462000956
#SBATCH --output=%x.%J.out
#SBATCH --partition=debug
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=64
#SBATCH --time=00:01:00

srun ./hello
