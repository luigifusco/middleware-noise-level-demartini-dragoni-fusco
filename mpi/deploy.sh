#!/bin/bash

truncate -s 0 /app/hostfile

for HOST in $@
do
    scp /app/sim /app/simulation.toml "${HOST}:/app/"
    echo "${HOST} slots=2" >> /app/hostfile
done

cat /app/hostfile

mpirun -v --hostfile /app/hostfile /app/sim /app/simulation.toml