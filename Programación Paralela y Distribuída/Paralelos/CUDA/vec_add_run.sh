#!/bin/sh
#PBS -N test
#PBS -l nodes=1:ppn=1:gpus=1

cd $PBS_O_WORKDIR

./vecAdd
