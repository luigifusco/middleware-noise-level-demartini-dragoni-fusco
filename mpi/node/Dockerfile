FROM debian:11

RUN apt-get update \
    && apt-get install -y --no-install-recommends openmpi-bin libopenmpi3 librdkafka1 openssh-server

ENV USER sim

RUN mkdir /var/run/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

RUN useradd -m ${USER}
# Pre generated ssh keys for docker simulation

ENV SSHDIR /home/${USER}/.ssh
RUN mkdir -p ${SSHDIR}
ADD ssh/config ${SSHDIR}/config
ADD ssh/sim ${SSHDIR}/id_ed25519
ADD ssh/sim.pub ${SSHDIR}/id_ed25519.pub
ADD ssh/sim.pub ${SSHDIR}/authorized_keys

RUN chmod -R 600 ${SSHDIR}/* && \
    chown -R ${USER}:${USER} ${SSHDIR} && \
    mkdir /app && chown -R ${USER}:${USER} /app

CMD ["/usr/sbin/sshd", "-D"]