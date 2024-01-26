FROM ubuntu:22.04

# 安装相关的软件包
RUN apt-get update &&apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui libssl-dev gcc-arm-linux-gnueabi

WORKDIR /root
RUN mkdir /usr/local/lib/ && mkdir /usr/local/include/
COPY /usr/local/lib/ /usr/local/lib/
COPY /usr/local/include/ /usr/local/include/


ENTRYPOINT ["bash", "/root/tbox/make.sh"]

