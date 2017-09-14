
**emoskit是一个比较简洁小巧的RPC框架，编译生成的库只有400多K，目前只支持Linux平台。**

作者： Little Zhou

联系人： little.zhou@foxmail.com

emoskit[![Build Status]https://api.travis-ci.org/git-littlezhou/emoskit.svg?branch=master](https://travis-ci.org/git-littlezhou/emoskit)

# 总览
  - 使用Protobuf作为IDL，用于描述RPC接口以及通信协议。
  - 基于proto文件自动生成Client以及Server接口，用于Client及Server的构建与实现。
  - 采用多线程模式，通过epoll管理请求的接入以及网络IO，工作线程采用可申缩的线程池，IO线程与工作线程通过内存无锁化循环队列进行交互。
  - 提供简易的Client和Server配置方式。

# 局限
  - 目前只支持TCP协议，后续可用会加入UDP等的支持。
  - 还没有提供过载保护、自适应拒绝请求等保护功能

# 如何编译
#### 编译环境
  - Linux.
  - G++-4.8及以上版本

#### 安装第三方库
emoskit必须依赖的第三方库Protobuf，在编译前，需要在系统上先安装好Protobuf 3.0或更新版本，也可执行目录下的install-protobuf.sh脚本进行安装。

#### emoikit编译安装方法
跟很多项目一样，进入emoskit根目录，执行：

    ./confiugre (--prefix=指定安装目录，默认安装到/usr/local目录下，--with-protobuf=指定protobuf所在安装目录，如果protobuf安装在系统目录或有设置环境变量则不需指定)
    make
    make install

# 如何使用
#### 编写proto文件
以下面hello.proto文件为例：

```c++
syntax = "proto3";

package helloworld;

service Hello {
	rpc SayHello (HelloRequest) returns (HelloReply) {}
}

message HelloRequest {
	string name = 1;
}

message HelloReply {
	string message = 1;
}

```
#### 生成代码

```bash
(emoskit安装目录，有配置环境变量或者安装在系统目录的则只需调用emoskitc)/bin/emoskitc <-f proto file> <-d destination dir>
 根据选项解析用户提供的proto文件并生成代码:
    -f <proto file>             # 指定所用proto文件
	-d <dir>                    # 生成代码输出目录
	-I <dir>                    # 如果用户的proto文件依赖protobuf系统提供的.proto文件，则可以在这里指定include目录

example:
emoskitc -f hello.proto -d .

调用完emoskitc工具后，全部相关的代码会自动生成后放置在指定目录，调用make即可进行编译。

```

#### 补充Server端和Client端的代码
##### Server端(xxx_service_impl.cpp)

```c++
//以下是默认生成的Service代码，取消SayHello函数的注释，并根据自己需要实现其逻辑，可以参考框架自动生成的Echo函数的实现

#include <string>

#include "hello_service_impl.h"

::emoskit::Status
HelloServiceImpl::Echo(::emoskit::ServerContext* context,
	const ::google::protobuf::StringValue& request, ::google::protobuf::StringValue* response) {
	response->set_value(request.value());
	return Status::OK;
}

/*	You can uncomment and implement the functions	*/

/*
::emoskit::Status
HelloServiceImpl::SayHello(::emoskit::ServerContext* context,
	const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response) {}
*/

```

##### Client端(xxx_client.cpp)

```c++
//以下是默认生成的Client代码，可以参考框架自动生成的Echo方法自行实现自己的方法

#include <emoskit/client_context.h>

#include "hello_client.h"

using emoskit::ClientContext;
using emoskit::Status;

namespace helloworld {

	std::string
	HelloClient::Echo(const std::string& msg) {
		::google::protobuf::StringValue request;
		::google::protobuf::StringValue response;

		request.set_value(msg);

		ClientContext client_context;

		Status status = stub_->Echo(&client_context, request, &response);

		if (status.is_ok()) {
			return response.value();
		}
		else {
			emoskit_log(ERROR_LOG, "RPC failed, code: %d, message : %s", status.code(), status.message().c_str());
			return "RPC failed";
		}

	}
}

```

##### Server端配置说明 (xxx_server.conf)

```c++
[Server]
IP = 127.0.0.1                       #Server绑定IP地址
Port = 50060                         #Server绑定端口
EventLoopCnt = 3                     #Epoll loop的数量
WorkerPoolCorePoolSize = 6           #Worker池的初始大小
WorkerPoolMaxPoolSize = 9            #Worker不够用的时候所能增加到的最大数量
WorkerPoolKeepAliveTimeSeconds = 60  #新增加的Worker空闲超过指定时间没任务会自动关闭
CircbufSize = 1024                   #内存无锁化循环队列的大小，建议为2^n
 
[Log]
LogDir = ./log
LogLevel = 3                         #DEBUG:1, INFO:2, WARN:3, ERROR:4

```

##### Client端配置说明 (xxx_client.conf)

```c++
[Server]
ServerCount = 2                      #Server的数量，有多个Server的时候会从中随机挑一个进行RPC调用

[Server_0]
IP = 127.0.0.1
Port = 50060

[Server_1]
IP = 127.0.0.1
Port = 50060

```