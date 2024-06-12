#!/bin/bash
pwd
ls

apt-get update && apt upgrade -y

apt-get -y install wget
apt-get -y install libpaho-mqtt-dev
apt-get -y install libjansson-dev
apt-get -y install libgtest-dev
apt-get -y install lcov

apt install ca-certificates apt-transport-https software-properties-common lsb-release libboost-dev git -y

apt install python3.11 -y
python3.11 --version

echo "----------install deps end ----------"

echo "----------start build and intstall mockcpp ----------"

rm -rf ../mockcpp

git clone https://github.com/sinojelly/mockcpp.git ../mockcpp


cd ../mockcpp

sed -i 's/XUNIT_NAME=testngpp/XUNIT_NAME=gtest/g' build_install.sh
# sed -i 's/XUNIT_NAME=testngpp/XUNIT_NAME=gtest/g' build_install.sh

echo "----------cat mockcpp build_install.sh content  ----------"
cat build_install.sh
echo "----------cat mockcpp build_install.sh content End. will build mockcpp ----------"

XUNIT_HOME=/usr/src/gtest ./build_install.sh # gtest intall to this dir
ls -ll ../

echo "----------start build tbox ----------"
cd ../test_tools/mockcpp_install
cp lib/libmockcpp.a /usr/lib
cp -rf ./include/mockcpp /usr/include/mockcpp
cp -rf ./include/fake_boost /usr/include/fake_boost
cp -rf ./include/msinttypes /usr/include/msinttypes


echo "----------build tbox app for arm platform start -----------------------"

cd /root/tbox/cantomqtt

pwd

can_send="./can_send"
tbox="./tbox"

# check file exist
if [ -e "$can_send" ]; then
    rm -f "$can_send"
    echo "can_send deleted."
else
    echo "File does not exist."
fi


if [ -e "$tbox" ]; then
    rm -f "$tbox"
    echo "tbox deleted."
else
    echo "File does not exist."
fi

echo "----------build start------------------------------------------------"

# local debug
arm-linux-gnueabi-gcc -g -o can_send writecan.c control.c -ljansson -march=armv5te

# -g for gdb remote debug 
arm-linux-gnueabi-gcc -g -o tbox mqtt.c control.c collision.c -lpaho-mqtt3c -ljansson -march=armv5te
chmod -R 777 can_send
chmod -R 777 tbox

echo "----------build tbox app for arm platform end -----------------------"

echo "----------start build tbox for unit test ----------"

cd /root/tbox

ls

chmod -R 777 build
chmod -R 777 arts_out
rm -rf build/* 

cd build 
cmake -DCODE_COVERAGE=ON .. 
make

echo "----------build  end ----------"

ctest -V | tee ../arts_out/test_result.txt


echo "----------unit test end ----------"

lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info  # Filter out the coverage information of system libraries
lcov --list coverage.info | tee ../arts_out/coverage_result.txt  # View the coverage summary and save the results
genhtml coverage.info --output-directory out  # Generate HTML report
tar -zcvf ../arts_out/coverage_result_html_report.tar.gz out  # Compress the report

echo "----------all done ----------"


# how to use this sh file:
# build and exec unit test.
# docker run --name tbox_build --entrypoint /bin/bash --rm -v E:\work\project\hw-codearts\stage3-tbox\IoV-TBox:/root/tbox swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4 -c "/root/tbox/build_gtest.sh"
# Enter container and build exec unit test by manual: ./build_gtest.sh
# docker run --name tbox_build --entrypoint /bin/bash --rm -it -v E:\work\project\hw-codearts\stage3-tbox\IoV-TBox:/root/tbox swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4
# docker exec -it tbox_build /bin/bash  #enter container


