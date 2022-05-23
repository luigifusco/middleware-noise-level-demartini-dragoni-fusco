FROM debian:11

RUN apt-get update \
    && apt-get install -y openmpi-bin libopenmpi3 librdkafka1 openssh-server sudo

ENV USER sim

RUN mkdir /var/run/sshd
RUN echo 'root:${USER}' | chpasswd
RUN sed -i 's/PermitRootLogin without-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

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
    chown -R ${USER}:${USER} ${SSHDIR}

# USER ${USER}

CMD ["/usr/sbin/sshd", "-D"]