# Cpp-webserver

## 1.项目描述
Linux 下 C++ 轻量级 Web 服务器.
其主要功能是通过 HTTP 协议与客户端（通常是浏览器（Browser））进行通信，来接收，存储，处理来自客户端的 HTTP 请求，并对其请求做出 HTTP 响应，返回给客户端其请求的内容（文件、网页等）或返回一个 Error 信息。

## 2.所用技术
- 使用 线程池处理数据
- socket 通信
- epoll 监听文件描述符数据
- 使用状态机解析 HTTP 请求报文
- 同步 IO 模拟 Proactor 的并发模型


## 3.设计框架

<div align=center>
<img width="699" alt="image" src="https://user-images.githubusercontent.com/22310531/153018656-0d1f891d-7bf4-430d-b01c-e04bab9b69e5.png">
</div>

<div align=center>
<img width="273" alt="image" src="https://github.com/XuDaHaoRen/Cpp-webserver/blob/main/%E6%9C%8D%E5%8A%A1%E5%99%A8.png">
</div>




## 4.项目如何执行

```
1.将根目录的所有程序进行 g++ 操作，要带 pthread 参数
g++ *.cpp -o main
2../main 10000 
3.键入界面在浏览器中
IP:10000/index.html

```

## 5.压力测试
压力测试使用 webbench 
基本原理：Webbench 首先 fork 出多个子进程，每个子进程都循环做 web 访问测试，子进程把访问的结果通过 pipe 告诉父进程，父进程统计结果显示在控制台

```
1.保存路径
webbench.o 保存在 test_presure
2.运行 webbench 时先 make 一下
make
3.执行指令
./webbench -c 1000 -t 30 http://192.168.110.129:10000/index.html
-c 连接的客户端数
-t 时间
```
<img width="627" alt="image" src="https://user-images.githubusercontent.com/22310531/154081059-9ba0bcea-f44f-4cd6-85f8-3722429c9afe.png">
使用 1000 个进程连接，访问 5 s ;每分钟可以访问 360900 个 page;956385 bytes/s;其中有 30075 个请求成功，0 个失败


## 6.运行环境
Linux ： 20.04
浏览器：FireFox


## 7.参考资料
牛客网高并发服务器项目：https://www.nowcoder.com/courses/cover/live/504
