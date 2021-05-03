/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef SAMPLES_ROUTING_H
#define SAMPLES_ROUTING_H

#include "samples-routing-node.h"
#include <boost/bimap.hpp>
#include "ns3/ipv4-address.h"
#include <string>
#include <map>
#include <vector>

namespace ns3
{
    class SamplesRoutingNode;
    class SamplesRoutingNetDevice;

    extern boost::bimap<Ipv4Address, std::string> addr2Name;                                       //node address <---->node name
    extern boost::bimap<std::string, Ptr<SamplesRoutingNode>> name2node;                           //node name <--->node ptr
    extern std::map<Ptr<SamplesRoutingNode>, std::vector<Ptr<SamplesRoutingNetDevice>>> node2port; //node --->the vector for its netdevices
    extern uint32_t pkgseq;                                                                        //the next seq of packet
    extern uint32_t MTU;                                                                           //the packet size in bytes
    extern Time simulatorEndTime;                                                                  //the simulator end time
}

#endif /* SAMPLES_ROUTING_H */
