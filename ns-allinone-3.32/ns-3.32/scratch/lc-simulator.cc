#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/samples-routing-module.h"
#include "ns3/ipv4-address-generator.h"

#include <map>
#include <vector>
#include <random>
#include <time.h>
#include <fstream>

#include "json.hpp"
#include "csv.hpp"

using namespace ns3;
using namespace io;

using json = nlohmann::json;

/*******************************
 * Route calculation variables *
 *******************************/

struct Interface
{
    Ptr<SamplesRoutingNetDevice> device;
    Time delay;
    DataRate bandwidth;
    uint64_t queueSize;
};
std::map<Ptr<SamplesRoutingNode>, std::map<Ptr<SamplesRoutingNode>, Interface>> onewayOutDev;
std::map<Ptr<SamplesRoutingNode>, std::map<Ptr<SamplesRoutingNode>, std::vector<Ptr<SamplesRoutingNode>>>> nextHopTable;
std::map<Ptr<SamplesRoutingNode>, std::map<Ptr<SamplesRoutingNode>, Time>> pairDelay;
std::map<Ptr<SamplesRoutingNode>, std::map<Ptr<SamplesRoutingNode>, Time>> pairTxDelay;
std::map<Ptr<SamplesRoutingNode>, std::map<Ptr<SamplesRoutingNode>, DataRate>> pairBandwidth;

/***************************************
 * Help functions for simulation setup *
 ***************************************/

//configure
void ConfigForNet5(const json &conf);
void ConfigForMesh(const json &conf);

//setup router table
void SetupDestination(const json &conf);
void CalculateRoute();
void CalculateRoute(Ptr<SamplesRoutingNode> host);
void SetRoutingEntries();

/*******************
 * Trace Functions *
 *******************/

std::map<std::string, std::stringstream> logStreams;
std::string outputFolder = "";

void InitTrace();
void AppTrace(Ptr<SamplesRoutingPacket> p);
void RouterDropBytesTrace(Time interval, Time endTime);
void RouterOutputGateTrace(Ptr<SamplesRoutingRouter> rt, Ptr<SamplesRoutingPacket> p, int outGateIndex);

void QueueDropBytesTrace(Time interval, Time endTime);
void QueueRxBytesTrace(Time interval, Time endTime);
void QueueTxBytesTrace(Time interval, Time endTime);
void QueueLengthTrace(Time interval, Time endTime);
void PacketQueueTimeTrace(Ptr<SamplesRoutingQueue> q, Ptr<SamplesRoutingPacket> p, Time t);

void DoTrace(Time interval, Time endTime);
void DoLog();

NS_LOG_COMPONENT_DEFINE("LC-SIMULATION");

int main(int argc, char *argv[])
{
    LogComponentEnable("LC-SIMULATION", LOG_LOGIC);

    if (argc < 2)
    {
        NS_LOG_UNCOND("ERROR: No config file");
        return 1;
    }

    std::ifstream file(argv[1]);

    json conf = json::parse(file);

    //init global variables
    simulatorEndTime = Time(conf["SimulatorEndTime"].get<std::string>());

    NS_LOG_UNCOND("=======HOST=======");
    //ConfigForNet5(conf);
    ConfigForMesh(conf);
    NS_LOG_UNCOND("=======router=======");
    CalculateRoute();
    SetRoutingEntries();

    NS_LOG_UNCOND("=======Trace=======");
    outputFolder = conf["OutputFolder"];
    Time interval = Time(conf["TraceInterval"].get<std::string>());
    Time endTime = Time(conf["TraceEndTime"].get<std::string>());

    InitTrace();
    DoTrace(interval, endTime);

    NS_LOG_UNCOND("=======start=======");
    Simulator::Run();
    Simulator::Destroy();

    NS_LOG_UNCOND("=======Output=======");
    DoLog();
}

/***************************************
 * Help functions for simulation setup *
 ***************************************/

void SetupDestination(const json &conf)
{
    for (const auto &name : conf["DestinationNode"])
    {
        for (uint32_t i = 0; i < name2node.size(); i++)
        {
            std::string app_nodename = "host" + std::to_string(i);
            if (app_nodename == name)
                continue;

            const auto app_node = name2node.left.at(app_nodename);
            const auto node = name2node.left.at(name);
            for (uint32_t j = 0; j < app_node->GetAppSize(); j++)
            {
                const auto app = app_node->GetApplication(j);
                app->SetupDestAddr(node->GetAddress());
            }
            app_nodename = "";
        }
    }
}

void ConfigForNet5(const json &conf)
{
    uint32_t h_num = conf["Number"];
    std::string time = conf["AppSendInterval"];
    Time sendInterval = Time(time);

    for (uint32_t i = 0; i < h_num; i++)
    {
        //node
        std::string name = "host" + std::to_string(i);
        const Ptr<SamplesRoutingNode> node = CreateObject<SamplesRoutingNode>(Ipv4AddressGenerator::NextAddress(Ipv4Mask("255.0.0.0")));
        node->SetName(name);
        addr2Name.insert({node->GetAddress(), name});
        name2node.insert({name, node});

        //app
        Ptr<SamplesRoutingApp> app = CreateObject<SamplesRoutingApp>();
        app->SetSendInterval(sendInterval);
        node->AddApplication(app);
        node->AggregateObject(app);
        //router
        Ptr<SamplesRoutingRouter> router = CreateObject<SamplesRoutingRouter>();
        node->AddRouter(router);

        //register receive callback router--->app
        router->SetupRxCallBack(MakeCallback(&SamplesRoutingApp::HandleRx, app));
    }
    //netdevice
    std::string rt = conf["DataRate"];
    DataRate dev_rate = DataRate(rt);
    double tLow = conf["LinkDelayLow"];
    double tHigh = conf["LinkDelayHigh"];

    uint32_t seed = conf["seed"];
    std::default_random_engine dre(seed);
    std::uniform_real_distribution<double> dis(tLow, tHigh); //generate delay randomly (ms)
    Time delay = Time(dis(dre));

    io::CSVReader<2> linkConfig(conf["LinkConfigFile"]);
    linkConfig.read_header(io::ignore_no_column, "FromNode", "ToNode");

    //queue
    uint32_t capacity = conf["QueueCapacity"];

    while (true)
    {
        std::string fromNode, toNode;
        if (!linkConfig.read_row(fromNode, toNode))
            break;

        const auto snode = name2node.left.at(fromNode);
        const auto dnode = name2node.left.at(toNode);

        //netdevice
        Ptr<SamplesRoutingNetDevice> sdev = CreateObject<SamplesRoutingNetDevice>();
        Ptr<SamplesRoutingNetDevice> ddev = CreateObject<SamplesRoutingNetDevice>();
        node2port[snode].push_back(sdev);
        node2port[dnode].push_back(ddev);

        //queue
        Ptr<SamplesRoutingQueue> squeue = CreateObject<SamplesRoutingQueue>(capacity);
        Ptr<SamplesRoutingQueue> dqueue = CreateObject<SamplesRoutingQueue>(capacity);

        //setup for netdevice
        sdev->SetQueue(squeue);
        ddev->SetQueue(dqueue);
        sdev->SetDataRate(dev_rate);
        ddev->SetDataRate(dev_rate);

        //channel
        Ptr<SamplesRoutingChannel> ch = CreateObject<SamplesRoutingChannel>();
        ch->SetDelay(delay);

        //dev attach to channel
        sdev->Attach(ch);
        ddev->Attach(ch);

        //register callback
        sdev->SetRxCallBack(MakeCallback(&SamplesRoutingRouter::HandleMsg, snode->GetRouter()));
        ddev->SetRxCallBack(MakeCallback(&SamplesRoutingRouter::HandleMsg, dnode->GetRouter()));

        //setup onewayOutDev
        onewayOutDev[snode][dnode] = {
            .device = sdev,
            .delay = delay,
            .bandwidth = dev_rate};
        onewayOutDev[dnode][snode] = {
            .device = ddev,
            .delay = delay,
            .bandwidth = dev_rate};
    }

    //setup dev for every node
    for (uint32_t i = 0; i < h_num; i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto node = name2node.left.at(name);

        for (uint32_t j = 0; j < node2port[node].size(); j++)
        {
            const auto dev = node2port[node][j];
            node->AddDevice(dev);
        }
    }

    //setup destination host address for every host
    SetupDestination(conf);
}

void ConfigForMesh(const json &conf)
{
    uint32_t hight = conf["Hight"];
    uint32_t width = conf["Width"];

    //app configure
    std::string time = conf["AppSendInterval"];
    Time sendInterval = Time(time);

    //dev configure
    std::string rt = conf["DataRate"];
    DataRate dev_rate = DataRate(rt);

    //queue configure
    uint32_t capacity = conf["QueueCapacity"];

    //link configure
    double dly = conf["LinkDelay"];
    Time delay = Time(dly);

    //host
    for (uint32_t i = 0; i < hight; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            //node
            std::string name = "host" + std::to_string(i * width + j);
            const Ptr<SamplesRoutingNode> node = CreateObject<SamplesRoutingNode>(Ipv4AddressGenerator::NextAddress(Ipv4Mask("255.0.0.0")));
            node->SetName(name);
            addr2Name.insert({node->GetAddress(), name});
            name2node.insert({name, node});

            //app
            Ptr<SamplesRoutingApp> app = CreateObject<SamplesRoutingApp>();
            app->SetSendInterval(sendInterval);
            node->AddApplication(app);
            node->AggregateObject(app);
            //router
            Ptr<SamplesRoutingRouter> router = CreateObject<SamplesRoutingRouter>();
            node->AddRouter(router);

            //register receive callback router--->app
            router->SetupRxCallBack(MakeCallback(&SamplesRoutingApp::HandleRx, app));

            //device--4
            Ptr<SamplesRoutingNetDevice> dev1 = CreateObject<SamplesRoutingNetDevice>();
            Ptr<SamplesRoutingNetDevice> dev2 = CreateObject<SamplesRoutingNetDevice>();
            Ptr<SamplesRoutingNetDevice> dev3 = CreateObject<SamplesRoutingNetDevice>();
            Ptr<SamplesRoutingNetDevice> dev4 = CreateObject<SamplesRoutingNetDevice>();
            node->AddDevice(dev1);
            node->AddDevice(dev2);
            node->AddDevice(dev3);
            node->AddDevice(dev4);
            node2port[node].push_back(dev1);
            node2port[node].push_back(dev2);
            node2port[node].push_back(dev3);
            node2port[node].push_back(dev4);

            //queue
            Ptr<SamplesRoutingQueue> queue1 = CreateObject<SamplesRoutingQueue>(capacity);
            Ptr<SamplesRoutingQueue> queue2 = CreateObject<SamplesRoutingQueue>(capacity);
            Ptr<SamplesRoutingQueue> queue3 = CreateObject<SamplesRoutingQueue>(capacity);
            Ptr<SamplesRoutingQueue> queue4 = CreateObject<SamplesRoutingQueue>(capacity);

            //setup for netdevice
            dev1->SetDataRate(dev_rate);
            dev2->SetDataRate(dev_rate);
            dev3->SetDataRate(dev_rate);
            dev4->SetDataRate(dev_rate);
            dev1->SetQueue(queue1);
            dev2->SetQueue(queue2);
            dev3->SetQueue(queue3);
            dev4->SetQueue(queue4);
        }
    }

    //link
    for (uint32_t i = 0; i < hight; i++)
    {
        if (i == hight - 1)
            break;
        for (uint32_t j = 0; j < width; j++)
        {
            if (i == hight - 1 || j == width - 1)
                break;
            std::string name1 = "host" + std::to_string(i * width + j);
            std::string name2 = "host" + std::to_string((i + 1) * width + j);
            std::string name3 = "host" + std::to_string(i * width + j + 1);

            const auto node1 = name2node.left.at(name1);
            const auto node2 = name2node.left.at(name2);
            const auto node3 = name2node.left.at(name3);

            //channel
            Ptr<SamplesRoutingChannel> ch1 = CreateObject<SamplesRoutingChannel>();
            ch1->SetDelay(delay);
            Ptr<SamplesRoutingChannel> ch2 = CreateObject<SamplesRoutingChannel>();
            ch2->SetDelay(delay);

            const auto node1_dev0 = node2port[node1][0];
            const auto node2_dev1 = node2port[node2][1];
            const auto node1_dev2 = node2port[node1][2];
            const auto node3_dev3 = node2port[node3][3];

            //dev attach to channel
            node1_dev0->Attach(ch1);
            node2_dev1->Attach(ch1);
            node1_dev2->Attach(ch2);
            node3_dev3->Attach(ch2);

            //register callback
            node1_dev0->SetRxCallBack(MakeCallback(&SamplesRoutingRouter::HandleMsg, node1->GetRouter()));
            node2_dev1->SetRxCallBack(MakeCallback(&SamplesRoutingRouter::HandleMsg, node2->GetRouter()));
            node1_dev2->SetRxCallBack(MakeCallback(&SamplesRoutingRouter::HandleMsg, node1->GetRouter()));
            node3_dev3->SetRxCallBack(MakeCallback(&SamplesRoutingRouter::HandleMsg, node3->GetRouter()));

            //setup onewayOutDev
            onewayOutDev[node1][node2] = {
                .device = node1_dev0,
                .delay = delay,
                .bandwidth = dev_rate};
            onewayOutDev[node2][node1] = {
                .device = node2_dev1,
                .delay = delay,
                .bandwidth = dev_rate};
            onewayOutDev[node1][node3] = {
                .device = node1_dev2,
                .delay = delay,
                .bandwidth = dev_rate};
            onewayOutDev[node3][node1] = {
                .device = node3_dev3,
                .delay = delay,
                .bandwidth = dev_rate};
        }
    }

    //setup destination host for every node
    SetupDestination(conf);
}
/*********************
 * Route calculation *
 *********************/
void CalculateRoute()
{
    for (uint32_t i = 0; i < name2node.size(); i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto host = name2node.left.at(name);

        CalculateRoute(host);
    }
}

void CalculateRoute(Ptr<SamplesRoutingNode> host)
{
    std::vector<Ptr<SamplesRoutingNode>> bfsQueue;          // Queue for the BFS
    std::map<Ptr<SamplesRoutingNode>, int> distances;       // Distance from the host to each node
    std::map<Ptr<SamplesRoutingNode>, Time> delays;         // Delay from the host to each node
    std::map<Ptr<SamplesRoutingNode>, Time> txDelays;       // Transmit delay from the host to each node
    std::map<Ptr<SamplesRoutingNode>, DataRate> bandwidths; // Bandwidth from the host to each node
    // Init BFS
    bfsQueue.push_back(host);
    distances[host] = 0;
    delays[host] = Time(0);
    txDelays[host] = Time(0);
    bandwidths[host] = DataRate(UINT64_MAX);
    // Do BFS
    for (size_t i = 0; i < bfsQueue.size(); i++)
    {
        const auto currNode = bfsQueue[i];
        for (const auto &next : onewayOutDev[currNode])
        {
            const auto nextNode = next.first;
            const auto nextInterface = next.second;
            // If 'nextNode' have not been visited.
            if (distances.find(nextNode) == distances.end())
            {
                distances[nextNode] = distances[currNode] + 1;
                delays[nextNode] = delays[currNode] + nextInterface.delay;
                txDelays[nextNode] =
                    txDelays[currNode] + nextInterface.bandwidth.CalculateBytesTxTime(MTU);
                bandwidths[nextNode] = std::min(bandwidths[currNode], nextInterface.bandwidth);

                bfsQueue.push_back(nextNode);
            }
            // if 'currNode' is on the shortest path from 'nextNode' to 'host'.
            if (distances[currNode] + 1 == distances[nextNode])
            {
                nextHopTable[nextNode][host].push_back(currNode);
            }
        }
    }
    for (const auto &it : delays)
        pairDelay[it.first][host] = it.second;
    for (const auto &it : txDelays)
        pairTxDelay[it.first][host] = it.second;
    for (const auto &it : bandwidths)
        pairBandwidth[it.first][host] = it.second;
}

void SetRoutingEntries()
{
    // For each node
    for (const auto &nextHopEntry : nextHopTable)
    {
        const auto fromNode = nextHopEntry.first;
        const auto toNodeTable = nextHopEntry.second;
        for (const auto &toNodeEntry : toNodeTable)
        {
            // The destination node
            const auto toNode = toNodeEntry.first;
            // The next hops towards the destination
            const auto nextNodeTable = toNodeEntry.second;
            // The IP address of the destination
            Ipv4Address dstAddr = toNode->GetAddress();
            for (const auto &nextNode : nextNodeTable)
            {
                const auto device = onewayOutDev[fromNode][nextNode].device;

                const auto router = fromNode->GetRouter();
                router->BuildRouterTable(dstAddr, device);
            }
        }
    }
}

/*******************
 * Trace Functions *
 *******************/
void InitTrace()
{
    logStreams["AppTrace"]
        << "PacketName,Size,CreateTime,EndTime,Duration,HopCount,SourceAddress\n";

    logStreams["RouterDropBytes"] << "Time,NodeName,DropBytes\n";
    logStreams["RouterOutputGate"] << "Time,RouterName,PkgName,OutputIf\n";

    logStreams["QueueDropBytes"] << "Time,NodeName,Port,QueueIndex,DropBytes\n";
    logStreams["QueueRxBytes"] << "Time,NodeName,Port,QueueIndex,RxBytes\n";
    logStreams["QueueTxBytes"] << "Time,NodeName,Port,QueueIndex,TxBytes\n";
    logStreams["QueueLength"] << "Time,NodeName,Port,QueueIndex,Length\n";
    logStreams["PacketQueueTime"] << "NodeName,Port,QueueIndex,PkgName,QueueTime\n";

    //regeister for app trace
    for (uint32_t i = 0; i < name2node.size(); i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto node = name2node.left.at(name);
        for (uint32_t j = 0; j < node->GetNApplications(); j++)
        {
            const auto app = node->GetApplication(j);
            app->TraceConnectWithoutContext("PacketRxComplete", MakeCallback(&AppTrace));
            const auto router = node->GetRouter();
            router->TraceConnectWithoutContext("RouterOutputGateTrace", MakeCallback(&RouterOutputGateTrace));
        }

        for (uint32_t v = 0; v < node->GetNDevices(); v++)
        {
            const auto dev = node->GetDevice(v);
            const auto que = dev->GetQueue();
            que->TraceConnectWithoutContext("PkgQueueTimeTrace", MakeCallback(&PacketQueueTimeTrace));
        }
    }
}

void AppTrace(Ptr<SamplesRoutingPacket> p)
{
    const auto Name = p->GetName();
    const auto endTime = Simulator::Now();
    const auto duration = endTime - p->GetCreateTime();
    const auto hopCount = p->GetHopCount();
    const auto sourceAddr = p->GetSourceIp();
    logStreams["AppTrace"] << Name << "," << p->GetSize() << "," << p->GetCreateTime() << ","
                           << endTime << "," << duration.GetNanoSeconds() << "," << hopCount
                           << "," << sourceAddr << "\n";
}

void RouterDropBytesTrace(Time interval, Time endTime)
{
    for (uint32_t i = 0; i < name2node.size(); i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto host = name2node.left.at(name);
        const auto router = host->GetRouter();
        uint32_t dropBytes = router->GetDropBytes();
        logStreams["RouterDropBytes"] << Simulator::Now() << "," << name << "," << dropBytes << "\n";
    }
    if (Simulator::Now() < endTime)
        Simulator::Schedule(interval, &RouterDropBytesTrace, interval, endTime);
}

void RouterOutputGateTrace(Ptr<SamplesRoutingRouter> rt, Ptr<SamplesRoutingPacket> p, int outGateIndex)
{
    const auto rtname = rt->GetNode()->GetName();
    const auto pkgname = p->GetName();
    logStreams["RouterOutputGate"] << Simulator::Now() << "," << rtname << "," << pkgname << "," << outGateIndex << "\n";
}

void QueueDropBytesTrace(Time interval, Time endTime)
{
    for (uint32_t i = 0; i < name2node.size(); i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto host = name2node.left.at(name);
        for (uint32_t j = 0; j < host->GetNDevices(); j++)
        {
            const auto dev = host->GetDevice(j);
            uint32_t gateIndex = dev->GetIfIndex();
            const auto que = dev->GetQueue();
            uint32_t dropBytes = que->GetDropBytes();
            logStreams["QueueDropBytes"] << Simulator::Now() << "," << name << "," << gateIndex << "," << 0 << "," << dropBytes << "\n";
        }
    }
    if (Simulator::Now() < endTime)
        Simulator::Schedule(interval, &QueueDropBytesTrace, interval, endTime);
}

void QueueRxBytesTrace(Time interval, Time endTime)
{
    for (uint32_t i = 0; i < name2node.size(); i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto host = name2node.left.at(name);
        for (uint32_t j = 0; j < host->GetNDevices(); j++)
        {
            const auto dev = host->GetDevice(j);
            uint32_t gateIndex = dev->GetIfIndex();
            uint32_t rxBytes = dev->GetRxBytes();
            logStreams["QueueRxBytes"] << Simulator::Now() << "," << name << "," << gateIndex << "," << 0 << "," << rxBytes << "\n";
        }
    }
    if (Simulator::Now() < endTime)
        Simulator::Schedule(interval, &QueueRxBytesTrace, interval, endTime);
}

void QueueTxBytesTrace(Time interval, Time endTime)
{
    for (uint32_t i = 0; i < name2node.size(); i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto host = name2node.left.at(name);
        for (uint32_t j = 0; j < host->GetNDevices(); j++)
        {
            const auto dev = host->GetDevice(j);
            uint32_t gateIndex = dev->GetIfIndex();
            uint32_t txBytes = dev->GetTxBytes();
            logStreams["QueueTxBytes"] << Simulator::Now() << "," << name << "," << gateIndex << "," << 0 << "," << txBytes << "\n";
        }
    }
    if (Simulator::Now() < endTime)
        Simulator::Schedule(interval, &QueueTxBytesTrace, interval, endTime);
}

void QueueLengthTrace(Time interval, Time endTime)
{
    for (uint32_t i = 0; i < name2node.size(); i++)
    {
        std::string name = "host" + std::to_string(i);
        const auto host = name2node.left.at(name);
        for (uint32_t j = 0; j < host->GetNDevices(); j++)
        {
            const auto dev = host->GetDevice(j);
            uint32_t gateIndex = dev->GetIfIndex();
            const auto que = dev->GetQueue();
            uint32_t len = que->GetQueueLength();
            logStreams["QueueLength"] << Simulator::Now() << "," << name << "," << gateIndex << "," << 0 << "," << len << "\n";
        }
    }
    if (Simulator::Now() < endTime)
        Simulator::Schedule(interval, &QueueLengthTrace, interval, endTime);
}

void PacketQueueTimeTrace(Ptr<SamplesRoutingQueue> q, Ptr<SamplesRoutingPacket> p, Time t)
{
    const auto dev = q->GetDev();
    const auto node = dev->GetSNode();
    std::string node_name = node->GetName();
    uint32_t gateIndex = dev->GetIfIndex();
    std::string pkg_name = p->GetName();
    logStreams["PacketQueueTime"] << node_name << "," << gateIndex << "," << 0 << "," << pkg_name << "," << t << "\n";
}

void DoTrace(Time interval, Time endTime)
{
    RouterDropBytesTrace(interval, endTime);

    QueueDropBytesTrace(interval, endTime);
    QueueRxBytesTrace(interval, endTime);
    QueueTxBytesTrace(interval, endTime);
    QueueLengthTrace(interval, endTime);
}
/*****************
 * Log Functions *
 *****************/

void DoLog()
{
    for (const auto &streamItem : logStreams)
    {
        const auto &name = streamItem.first;
        const auto &ss = streamItem.second;
        const std::string filePath = outputFolder + "/" + name + ".csv";
        std::ofstream file(filePath);
        file << ss.str();
        file.close();
    }
}