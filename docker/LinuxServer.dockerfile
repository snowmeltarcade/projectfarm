FROM ubuntu:20.04 as dev

# don't ask for user input when installing various packages
# https://askubuntu.com/questions/909277/avoiding-user-interaction-with-tzdata-when-installing-certbot-in-a-docker-contai
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y && \
    apt-get autoclean -y && \
    apt-get autoremove -y && \
    apt-get install -y apt-utils software-properties-common python3.8 git cmake curl wget && \
    # install the clang dependencies for the compiler we use
    bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

WORKDIR /src

RUN git clone https://github.com/snowmeltarcade/projectfarm.git && \
    cd ./projectfarm && \
    python3 build.py --install-assets --install-dependencies --archive-name=projectfarm

FROM ubuntu:20.04 as prod

COPY --from=dev /src/projectfarm/archives/ /temp/

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /usr/projectfarm/

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y && \
    apt-get autoclean -y && \
    apt-get autoremove -y && \
    apt-get install -y zip unzip && \
    unzip /temp/projectfarm.zip && \
    # remove the files we don't need to run the server
    rm /temp/projectfarm.zip && \
    rm -rf /usr/projectfarm/client/ && \
    rm -rf /usr/projectfarm/server/

CMD ["/usr/projectfarm/latest/bin/projectfarm_server"]