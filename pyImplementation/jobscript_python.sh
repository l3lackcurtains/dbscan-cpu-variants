#!/bin/bash

#SBATCH --job-name=dbscan_py
#SBATCH --output=/scratch/mp2525/dbscan_py.txt
#SBATCH --error=/scratch/mp2525/dbscan_py.err
#SBATCH --time=720:00
#SBATCH --mem=0
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1

module load anaconda3

srun python ./dbscan.py