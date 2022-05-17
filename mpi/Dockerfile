FROM debian:11

RUN apt-get update \
    && apt-get install -y build-essential pkg-config openmpi-bin libopenmpi-dev librdkafka-dev

WORKDIR /app

COPY src/ ./src/
COPY Makefile ./

RUN useradd sim && chown -R sim:sim .
USER sim

RUN make

ENTRYPOINT [ "mpirun", "-np", "6", "target/sim", "simulation.toml" ]