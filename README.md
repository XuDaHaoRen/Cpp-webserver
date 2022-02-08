# Cpp-webserver

## 1.项目描述
Linux 下 C++ 轻量级 Web 服务器.
其主要功能是通过 HTTP 协议与客户端（通常是浏览器（Browser））进行通信，来接收，存储，处理来自客户端的 HTTP 请求，并对其请求做出 HTTP 响应，返回给客户端其请求的内容（文件、网页等）或返回一个 Error 信息。

## 2.所用技术
- 使用 **线程池处理数据
- socket 通信
- epoll 监听文件描述符数据
- 使用状态机解析 HTTP 请求报文
- 同步 IO 模拟 Proactor 的并发模型


## 3.设计框架
<img width="699" alt="image" src="https://user-images.githubusercontent.com/22310531/153018656-0d1f891d-7bf4-430d-b01c-e04bab9b69e5.png">

## 4.项目如何执行


## 5.运行环境
Linux ： 20.04
浏览器：FireFox

## 6.参考资料
牛客网高并发服务器项目：https://www.nowcoder.com/courses/cover/live/504
