# routing-simulator
## Samples-routing模块中各文件介绍
• Samples-routing-app.cc & samples-routing-app.h
	功能对应于omnetpp程序中的App.cc
	主要函数：
		a. void HandleRx(Ptr<SamplesRoutingPacket> p)；负责处理接收到的数据包
		b. void StartApplication(); 负责产生并发送数据包
• samples-routing-router.cc & samples-routing-router.h
	功能对应于omnetpp程序中的Routing.cc
	主要函数：
		a. void HandleMsg(Ptr<SamplesRoutingPacket> p); 负责转发主机中app产生的数据包
		b. void BuildRouterTable(Ipv4Address dstIp, Ptr<SamplesRoutingNetDevice> outPort); 负责构建该router对象对应的路由表 m_routerTable; //dest ip ---> net device
		
• Samples-routing-net-device.cc & samples-routing-net-device.h
	作为node（主机）发往链路的设备
	主要函数：
		a.  void TransmitStart(Ptr<SamplesRoutingPacket> p); 链路空闲时将数据包转发到链路上，否则将数据包入队
		b. void CompleteTransimit();结束一个数据包的转发，如果队列不空继续发送队列中其他数据包
		c. void Receive(Ptr<SamplesRoutingPacket> p); 接收到一个数据包，使用回调传递给主机的router处理
• samples-routing-queue.cc & samples-routing-queue.h
	默认一个net device包含一个queue
	主要函数：
		a.  void InqueuPkg(Ptr<SamplesRoutingPacket> p); 入队列一个数据包
		b. Ptr<SamplesRoutingPacket> DequeuePkg(); 出队列一个数据包
• Samples-routing-node.cc & samples-routing-node.h
	一个主机的抽象
	项目默认一个主机加一个application，一个router，若干net-device
• Samples-routing-channel.cc & samples-routing-channel.h
	发送数据包的链路
	主要函数：
		a.  bool TransmitStart(Ptr<SamplesRoutingPacket> p, Ptr<SamplesRoutingNetDevice> src, Time txTime);将一个数据包从链路一端传递到另一端
		b.  void Attach(Ptr<SamplesRoutingNetDevice> device); 一个channel两端各关联一个net-device
• samples-routing-packet.cc & samples-routing-packet.h
	数据包
• Samples-routing.cc & samples-routing.h
	声明一些全局变量
## 仿真文件介绍
	• Scratch/lc-simulator.cc为仿真入口
	• Ns3_config文件夹下是各网络拓扑的配置文件
	• Ns3_log文件夹下是各网络拓扑类型的运行跟踪变量输出文件
## 编译程序
    第一次使用./waf命令前需要先运行./waf configure
    运行./waf命令对程序进行编译
## 运行程序
    在文件夹routing-simulator/ns-allinone-3.32/ns-3.32/下运行命令：./waf --run "scratch/lc-simulator ns3_config/mesh/config.json" 得到网络类型为mesh的运行结果存于ns3_log/mesh/目录下
