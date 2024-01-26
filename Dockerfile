FROM ubuntu:22.04

# 安装相关的软件包
RUN apt-get update && apt-get install  -y --no-install-recommends build-essential gcc make cmake cmake-gui cmake-curses-gui libssl-dev gcc-arm-linux-gnueabi libc6-dev

WORKDIR /root
RUN [ -d /usr/local/ ] || mkdir -p /usr/local/ && mkdir -p /usr/local/lib/
COPY ./lib/* /usr/local/lib/
COPY ./lib/* /usr/lib/


ENTRYPOINT ["bash", "/root/tbox/make.sh"]

