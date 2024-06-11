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

# docker run --name cpp_build -d --rm -p 9422:22 -v C:\Users\limin\.ssh:/home/smartide/.ssh -v E:\work\project\hw-codearts\stage3-tbox\c-cpp-unittest-coverage-demo:/home/project/c-cpp-unittest-coverage-demo registry.cn-hangzhou.aliyuncs.com/smartide/smartide-cpp-v2:latest
# docker exec cpp_build /bin/bash -c "/home/project/c-cpp-unittest-coverage-demo/build_gtest.sh"


# docker run --name cpp_build  --entrypoint /bin/bash --rm -v E:\work\project\hw-codearts\stage3-tbox\c-cpp-unittest-coverage-demo:/home/project/c-cpp-unittest-coverage-demo tbox-compiler:v5 -c "/home/project/c-cpp-unittest-coverage-demo/build_gtest.sh"

# 用这个编译/执行测试.
# docker run --name tbox_build --entrypoint /bin/bash --rm -v E:\work\project\hw-codearts\stage3-tbox\IoV-TBox:/root/tbox swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4 -c "/root/tbox/build_gtest.sh"

# docker run --name tbox_build --entrypoint /bin/bash --rm -it -v E:\work\project\hw-codearts\stage3-tbox\IoV-TBox:/root/tbox swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4


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


## cd /root/workspace/IoV-TBox

echo "----------start build tbox ----------"

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
lcov --remove coverage.info '/usr/*' --output-file coverage.info  # 过滤掉系统库的覆盖率信息
lcov --list coverage.info | tee ../arts_out/coverage_result.txt  # 查看覆盖率概要 保存结果
genhtml coverage.info --output-directory out  # 生成HTML报告
tar -zcvf ../arts_out/coverage_result_html_report.tar.gz out  # 压缩报告

echo "----------all done ----------"

# mkdir ../mockcpp_source && cd ../mockcpp_source
# tar -xvzf mockcpp-2.6.tar.gz && cd mockcpp


# #docker run --name mock --entrypoint /bin/bash -it -v E:\work\project\hw-codearts\stage3-tbox\:/root/workspace swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4
# apt-get update && apt upgrade 
# # apt-get -y install libgtest-dev
# apt install ca-certificates apt-transport-https software-properties-common lsb-release libboost-dev -y

# apt install python3.11 -y
# python3.11 --version

# # git clone https://github.com/sinojelly/mockcpp.git



# cp lib/libg*.a /usr/lib
# cp -rf ../googletest/include/gtest /usr/include/gtest
# cp -rf ../googlemock/include/gmock /usr/include/gmock

# cd ../../test_tools/mockcpp_install
# cp lib/libmockcpp.a /usr/lib
# cp -rf ./include/mockcpp /usr/include/mockcpp
# cp -rf ./include/fake_boost /usr/include/fake_boost
# cp -rf ./include/msinttypes /usr/include/msinttypes




