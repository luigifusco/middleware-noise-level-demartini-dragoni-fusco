FROM debian:11

RUN apt-get update \
    && apt-get install -y build-essential openmpi-bin librdkafka-dev

COPY Makefile .
COPY src/ ./src/

RUN make

ENTRYPOINT [ "mpirun", "-np", "6", "target/sim" ]