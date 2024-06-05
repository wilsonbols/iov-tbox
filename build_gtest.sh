#!/bin/bash
pwd
ls
apt-get update

apt-get -y install libpaho-mqtt-dev
apt-get -y install libjansson-dev
apt-get -y install libgtest-dev
apt-get -y install lcov

echo "----------install deps end ----------"

# docker run --name cpp_build -d --rm -p 9422:22 -v C:\Users\limin\.ssh:/home/smartide/.ssh -v E:\work\project\hw-codearts\stage3-tbox\c-cpp-unittest-coverage-demo:/home/project/c-cpp-unittest-coverage-demo registry.cn-hangzhou.aliyuncs.com/smartide/smartide-cpp-v2:latest
# docker exec cpp_build /bin/bash -c "/home/project/c-cpp-unittest-coverage-demo/build_gtest.sh"


# docker run --name cpp_build  --entrypoint /bin/bash --rm -v E:\work\project\hw-codearts\stage3-tbox\c-cpp-unittest-coverage-demo:/home/project/c-cpp-unittest-coverage-demo tbox-compiler:v5 -c "/home/project/c-cpp-unittest-coverage-demo/build_gtest.sh"

# 用这个编译/执行测试.
# docker run --name tbox_build --entrypoint /bin/bash --rm -v E:\work\project\hw-codearts\stage3-tbox\IoV-TBox:/root/tbox swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4 -c "/root/tbox/build_gtest.sh"
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
