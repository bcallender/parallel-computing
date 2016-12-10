#!/bin/bash
#SBATCH --mail-user=bmcallender@cs.uchicago.edu
#SBATCH --mail-type=ALL
#SBATCH --output=/home/bmcallender/slurm/slurm_out/%j.%N.stdout
#SBATCH --error=/home/bmcallender/slurm/slurm_out/%j.%N.stderr
#SBATCH --workdir=/home/bmcallender/slurm
#SBATCH --job-name=bmcallender-hw-n
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=10:00
#SBATCH --partition debug
#SBATCH --exclusive

./generator.py