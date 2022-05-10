CFLAGS=-Wall $(shell pkg-config --cflags --libs rdkafka) -lm

default:
	mkdir -p target
	mpicc.openmpi src/sim.c -o target/sim $(CFLAGS)

debug:
	mkdir -p target
	mpicc.openmpi -fsanitize=address -g src/sim.c -o target/sim $(CFLAGS)

run: default
	mpirun -np 6 target/sim