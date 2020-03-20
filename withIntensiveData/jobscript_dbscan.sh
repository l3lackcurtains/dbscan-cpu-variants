#!/bin/bash

#SBATCH --job-name=dbscan
#SBATCH --output=/scratch/mp2525/dbscan.txt
#SBATCH --error=/scratch/mp2525/dbscan.err
#SBATCH --time=120:00
#SBATCH --mem=80000
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1

g++ ./Dbscan.cpp -o ./Dbscan.exe

srun ./Dbscan.exe