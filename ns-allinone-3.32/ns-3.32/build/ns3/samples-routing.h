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

    extern boost::bimap<Ipv4Address, std::string> addr2Name;
    extern boost::bimap<std::string, Ptr<SamplesRoutingNode>> name2node;
    extern std::map<Ptr<SamplesRoutingNode>, std::vector<Ptr<SamplesRoutingNetDevice>>> node2port;
    extern uint32_t pkgseq;
    extern uint32_t MTU;
    extern Time simulatorEndTime;
}

#endif /* SAMPLES_ROUTING_H */
