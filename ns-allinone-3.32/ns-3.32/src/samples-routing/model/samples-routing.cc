/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "samples-routing.h"

namespace ns3
{

    boost::bimap<Ipv4Address, std::string> addr2Name; //all host ipv4address <----> host name
    boost::bimap<std::string, Ptr<SamplesRoutingNode>> name2node;
    std::map<Ptr<SamplesRoutingNode>, std::vector<Ptr<SamplesRoutingNetDevice>>> node2port;
    uint32_t pkgseq = 0;   //all packet number
    uint32_t MTU = 1500;   //default packet size
    Time simulatorEndTime; //simulator end time
} // namespace ns3
