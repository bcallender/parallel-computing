#!/bin/bash
#SBATCH --mail-user=bmcallender@uchicago.edu
#SBATCH --mail-type=ALL
#SBATCH --output=/home/bmcallender/slurm/slurm_out/%j.%N.stdout
#SBATCH --error=/home/bmcallender/slurm/slurm_out/%j.%N.stderr
#SBATCH --workdir=/home/bmcallender/slurm
#SBATCH --job-name=bmcallender-hw-n
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=120:00
#SBATCH --partition general
#SBATCH --exclusive

./exp4.py