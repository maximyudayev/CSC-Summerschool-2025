#!/bin/bash
#SBATCH --job-name=message-length
#SBATCH --account=project_462000956
#SBATCH --output=%x.%J.out
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:01:00

srun ./message-length
