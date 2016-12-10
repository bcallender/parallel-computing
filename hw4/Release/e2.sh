#!/bin/bash
#SBATCH --mail-user=bmcallender@uchicago.edu
#SBATCH --mail-type=ALL
#SBATCH --output=/home/bmcallender/slurm/slurm_out/%j.%N.stdout
#SBATCH --error=/home/bmcallender/slurm/slurm_out/%j.%N.stderr
#SBATCH --workdir=/home/bmcallender/slurm
#SBATCH --job-name=bmcallender-hw-2
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=180:00
#SBATCH --partition general
#SBATCH --exclusive

./exp2.py