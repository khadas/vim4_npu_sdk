FROM ubuntu:20.04
RUN dpkg --add-architecture i386
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y dist-upgrade && \
	DEBIAN_FRONTEND=noninteractive apt-get install -y sudo git python2 python3 libpython2.7-dev python3-pip locales libgl1-mesa-dev cmake
RUN locale-gen en_US.UTF-8
ENV LANG='en_US.UTF-8' LANGUAGE='en_US:en' LC_ALL='en_US.UTF-8' TERM=screen

# Switch to normal user
RUN useradd -c 'khadas' -m -d /home/khadas -s /bin/bash khadas
RUN sed -i -e '/\%sudo/ c \%sudo ALL=(ALL) NOPASSWD: ALL' /etc/sudoers
RUN usermod -a -G sudo khadas

USER khadas

RUN [ "/bin/bash" ]
