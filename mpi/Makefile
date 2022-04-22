
default:
	mkdir -p target
	mpicc.openmpi -Wall src/sim.c -lm -o target/sim

debug:
	mkdir -p target
	mpicc.openmpi -Wall -fsanitize=address -g src/sim.c -lm -o target/sim

run: default
	mpirun -np 6 target/sim