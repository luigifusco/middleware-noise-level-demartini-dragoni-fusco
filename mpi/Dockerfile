FROM debian:11

RUN apt-get update \
    && apt-get install -y gcc make pkg-config openmpi-bin libopenmpi-dev librdkafka-dev

ENV USER sim

RUN useradd -m ${USER}

# Pre generated ssh keys for docker simulation

ENV SSHDIR /home/${USER}/.ssh
RUN mkdir -p ${SSHDIR}
ADD node/ssh/config ${SSHDIR}/config
ADD node/ssh/sim ${SSHDIR}/id_ed25519
ADD node/ssh/sim.pub ${SSHDIR}/id_ed25519.pub
ADD node/ssh/sim.pub ${SSHDIR}/authorized_keys

RUN chmod -R 600 ${SSHDIR}/* && \
    chown -R ${USER}:${USER} ${SSHDIR}

RUN mkdir /app && chown -R ${USER}:${USER} /app

USER sim
WORKDIR /build

ADD src/ ./src/
ADD Makefile ./

RUN make

WORKDIR /app

RUN cp /build/target/sim /app/sim

ENTRYPOINT [ "mpirun", "sim", "simulation.toml" ]