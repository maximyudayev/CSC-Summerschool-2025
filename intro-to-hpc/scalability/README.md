# Scalability analysis

1. Build three dimensional heat equation solver under [heat-equation-3d](heat-equation-3d) with the provided [Makefile](heat-equation-3d/Makefile).
2. Run the program `heat_hip` with different number of GPUs / GCDs and investigate scalability by filling the table below.

Note 1: You can use the provided example `job.sh`. The code uses GPU-aware MPI communication, so you need to set the environment variable
```bash
export MPICH_GPU_SUPPORT_ENABLED=1
```
(set already in the `job.sh`).

Note 2: Each LUMI-G node has 8 GCDs ("GPUs" for Slurm job definition), so run the cases up to 8 GCDs on a single node (e.g., 4 GCDs is obtained with `--nodes=1 --ntasks-per-node=4 --gpus-per-node=4`)
and the larger cases on multiple full nodes (e.g., 16 GCDs is obtained with `--nodes=2 --ntasks-per-node=8 --gpus-per-node=8`).

Protip: you don't need to edit the `job.sh` for different GCD counts, but you can override the sbatch parameters through equivalent command line options. For example:
```bash
sbatch --job-name=n04 --nodes=1 --ntasks-per-node=4 --gpus-per-node=4 job.sh
sbatch --job-name=n16 --nodes=2 --ntasks-per-node=8 --gpus-per-node=8 job.sh
```


| GCDs | Runtime (s) | Resource cost (GCD-s) | Time spent in communication (s) | Time spent in computing (s) | Speedup | Parallel efficiency |
| ---: | ----------: | --------------------: | ------------------------------: | --------------------------: | ------: | ------------------: |
|   1  | 9.496543    | 9.496543              | 0.003412                        | 9.312358                    | 1       | 1                   |
|   2  | 5.008218    | 10.16236              | 0.149763                        | 4.736874                    | 1.90    | 0.95                |
|   4  | 2.861271    | 11.445084             | 0.326871                        | 2.445121                    | 3.32    | 0.83                |
|   8  | 1.631598    | 13.052784             | 0.294402                        | 1.268435                    | 5.82    | 0.73                |
|  16  | 1.136742    | 18.187872             | 0.394541                        | 0.680930                    | 8.35    | 0.52                |

Speedup $S$ and efficiency $E$ are defined as
```math
S = \frac{T_1}{T_n}
```
and
```math
E = \frac{T_1}{n T_n}
```
where $T_1$ is the runtime with a single GCD and $T_n$ is the runtime with $n$ GCDs.
The resource cost is $n T_n$, that is, the GCD-seconds or GCD-hours consumed.

As a rule of thumb, parallel efficiencies higher than 75% are usually considered reasonable.
This corresponds to a speedup of 1.5 when doubling the number GCDs/cores/nodes or to a speedup of 3 when quadrupling.
