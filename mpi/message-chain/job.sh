#!/bin/bash
#SBATCH --job-name=chain
#SBATCH --account=project_462000956
#SBATCH --output=%x.%J.out
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=5
#SBATCH --time=00:01:00

srun ./chain
