CC=mpicc.openmpi
CFLAGS=-Wall
LIBS= $(shell pkg-config --cflags --libs rdkafka) -lm 

ODIR = target
SDIR = src

_DEPS = toml/toml.h
OBJ = $(patsubst %,$(SDIR)/%,$(_DEPS))

_OBJ = toml/toml.o sim.o config.o utils.o physics.o communication.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

default: sim

debug:
	mkdir -p target
	mpicc.openmpi -fsanitize=address -g src/sim.c -o target/sim $(CFLAGS)

run: default
	mpirun -np 6 target/sim simulation.toml


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

sim: $(OBJ)
	$(CC) -o target/$@ $^ $(CFLAGS) $(LIBS)