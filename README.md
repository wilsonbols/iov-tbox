## 本地运行(以Linux为例)
- 本地环境参考这个：https://www.cnblogs.com/CltCj/articles/17661086.html
- 安装依赖：
```
mkdir ~/mqtt-client-sdk && cd ~/mqtt-client-sdk
git clone https://github.com/eclipse/paho.mqtt.c.git # 也可以使用国内的镜像:https://gitee.com/mirrors/paho.mqtt.c.git
cd paho.mqtt.c
mkdir build
cd build
cmake ..
make 
make install
```
- 或者命令行手动编译运行
```
mkdir manual-build && cd manual-build
cmake .. && make  ##构建 
cd .. && chmod -R 777 manual-build ##添加执行权限
cd manual-build
./IoV-Edge-CClient  ## 运行 crtl+C 退出

- 程序会订阅主 vehicle-warning 消息
- 程序会循环发布主题 vehicle-info, 会发两个，一个是读取vehicle-info.json文件的内容，一个是同样格式的静态内容

- 使用 http://www.emqx.io/online-mqtt-client 接收和发送消息以验证上面两个主题的消息

## 本代码库根据这个代码库修改而来
https://github.com/leansoftX/code-arts-mqtt-client-demo.git


