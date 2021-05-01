/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 Georgia Tech Research Corporation
 * Copyright (c) 2020 Nanjing University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: ChangLiu  <liuchang_1307@163.com>
 */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/network-module.h"
#include "samples-routing-app.h"
#include "samples-routing-packet.h"
#include "stdlib.h"
#include "ctime"
#include <string>
#include <cstdio>

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("SamplesRoutingApp");
    NS_OBJECT_ENSURE_REGISTERED(SamplesRoutingApp);

    TypeId
    SamplesRoutingApp::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::SamplesRoutingApp")
                                .SetParent<Application>()
                                .SetGroupName("samples-routing")
                                .AddConstructor<SamplesRoutingApp>()
                                .AddTraceSource("PacketRxComplete", "Completing Receiving a packet",
                                                MakeTraceSourceAccessor(&SamplesRoutingApp::m_packetRxCompleteTrace), "Ptr<SamplesRoutingPacket>");
        return tid;
    }

    SamplesRoutingApp::SamplesRoutingApp()
    {
        NS_LOG_FUNCTION_NOARGS();
    }

    SamplesRoutingApp::~SamplesRoutingApp()
    {
        NS_LOG_FUNCTION_NOARGS();
    }

    void SamplesRoutingApp::DoDispose()
    {
        NS_LOG_FUNCTION(this);
        m_node = 0;
        m_sentInterval = Time(0);
        Application::DoDispose();
    }

    void
    SamplesRoutingApp::DoInitialize(void)
    {
        NS_LOG_FUNCTION(this);
        m_startEvent = Simulator::Schedule(m_startTime, &SamplesRoutingApp::StartApplication, this);
        if (m_stopTime != TimeStep(0))
        {
            m_stopEvent = Simulator::Schedule(m_stopTime, &SamplesRoutingApp::StopApplication, this);
        }
        Application::DoInitialize();
    }

    void SamplesRoutingApp::SetSendInterval(Time t)
    {
        NS_LOG_FUNCTION(this << t);
        m_sentInterval = t;
    }

    void SamplesRoutingApp::SetNode(Ptr<SamplesRoutingNode> nd)
    {
        m_node = nd;
    }

    void SamplesRoutingApp::SetupDestAddr(Ipv4Address dstaddr)
    {
        m_destAddress.push_back(dstaddr);
    }

    void SamplesRoutingApp::StartApplication()
    {
        NS_LOG_FUNCTION(this);
        srand((unsigned)time(NULL));

        if (m_destAddress.size() == 0)
            return;
        int idx = rand() % (m_destAddress.size());

        Ipv4Address destAddr = m_destAddress[idx];
        Ipv4Address myAddr = m_node->GetAddress();

        Ptr<SamplesRoutingPacket> p = CreateObject<SamplesRoutingPacket>(MTU, myAddr, destAddr);

        p->SetCreateTime(Simulator::Now());
        std::string sname = addr2Name.left.at(myAddr);
        std::string dname = addr2Name.left.at(destAddr);
        std::string pkgname = "pk-" + sname + "-to-" + dname + "-#" + std::to_string(pkgseq++);
        NS_LOG_UNCOND("\nGenerate packet: " + pkgname);
        p->SetName(pkgname);

        m_node->GetRouter()->HandleMsg(p);
        if (Simulator::Now() < simulatorEndTime)
            Simulator::Schedule(m_sentInterval, &SamplesRoutingApp::StartApplication, this); //call for next send event
    }

    //TODO:fix
    void SamplesRoutingApp::StopApplication(void)
    {
        m_destAddress.clear();
        m_sentInterval = Time(0);
    }

    void SamplesRoutingApp::HandleRx(Ptr<SamplesRoutingPacket> p)
    {
        NS_LOG_FUNCTION(this);
        NS_LOG_UNCOND("\nReceive packet: " + p->GetName());
        m_packetRxCompleteTrace(p);
    }

    void SamplesRoutingApp::SetPkgRxCompleteTraceCallback(TracedCallback<Ptr<SamplesRoutingPacket>> cb)
    {
        m_packetRxCompleteTrace = cb;
    }

} // namespace ns3
