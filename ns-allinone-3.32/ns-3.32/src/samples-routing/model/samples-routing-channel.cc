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

#include "samples-routing-channel.h"

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("SamplesRoutingChannel");
    NS_OBJECT_ENSURE_REGISTERED(SamplesRoutingChannel);

    TypeId
    SamplesRoutingChannel::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::SamplesRoutingChannel")
                                .SetParent<Channel>()
                                .SetGroupName("samples-routing")
                                .AddConstructor<SamplesRoutingChannel>();

        return tid;
    }

    SamplesRoutingChannel::SamplesRoutingChannel() : m_delay(Seconds(0.)), m_nDevices(0)
    {
        NS_LOG_FUNCTION_NOARGS();
    }

    void SamplesRoutingChannel::DoDispose()
    {
        NS_LOG_FUNCTION(this);
        m_delay = Time(0);
        m_nDevices = 0;
        Object::DoDispose();
    }

    void
    SamplesRoutingChannel::Attach(Ptr<SamplesRoutingNetDevice> device)
    {
        NS_LOG_FUNCTION(this << device);
        NS_ASSERT_MSG(m_nDevices < N_DEVICES, "Only two devices permitted");
        NS_ASSERT(device != 0);

        m_link[m_nDevices++].m_src = device;
        //
        // If we have both devices connected to the channel, then finish introducing
        // the two halves and set the links to IDLE.
        //
        if (m_nDevices == N_DEVICES)
        {
            m_link[0].m_dst = m_link[1].m_src;
            m_link[1].m_dst = m_link[0].m_src;
            m_link[0].m_state = IDLE;
            m_link[1].m_state = IDLE;
        }
    }

    bool
    SamplesRoutingChannel::TransmitStart(Ptr<SamplesRoutingPacket> p, Ptr<SamplesRoutingNetDevice> src, Time txTime)
    {
        NS_LOG_FUNCTION(this << p << src);

        NS_ASSERT(m_link[0].m_state != INITIALIZING);
        NS_ASSERT(m_link[1].m_state != INITIALIZING);

        uint32_t wire = src == m_link[0].m_src ? 0 : 1;

        Simulator::Schedule(txTime + m_delay, &SamplesRoutingNetDevice::Receive, m_link[wire].m_dst, p);
        std::string fromname = m_link[wire].m_src->GetSNode()->GetName();
        std::string toname = m_link[wire].m_dst->GetSNode()->GetName();
        std::string msg = fromname + " -----> " + toname + "~~~~~" + p->GetName();
        NS_LOG_UNCOND("\n" + msg);
        return true;
    }

    std::size_t
    SamplesRoutingChannel::GetNDevices(void) const
    {
        NS_LOG_FUNCTION_NOARGS();
        return m_nDevices;
    }

    Ptr<SamplesRoutingNetDevice>
    SamplesRoutingChannel::GetSamplesRoutingNetDevice(std::size_t i) const
    {
        NS_LOG_FUNCTION_NOARGS();
        NS_ASSERT(i < 2);
        return m_link[i].m_src;
    }

    Ptr<NetDevice>
    SamplesRoutingChannel::GetDevice(std::size_t i) const
    {
        NS_LOG_FUNCTION_NOARGS();
        return GetSamplesRoutingNetDevice(i);
    }

    void SamplesRoutingChannel::SetDelay(Time dly)
    {
        m_delay = dly;
    }

    Time
    SamplesRoutingChannel::GetDelay(void) const
    {
        return m_delay;
    }

    Ptr<SamplesRoutingNetDevice>
    SamplesRoutingChannel::GetSource(uint32_t i) const
    {
        return m_link[i].m_src;
    }

    Ptr<SamplesRoutingNetDevice>
    SamplesRoutingChannel::GetDestination(uint32_t i) const
    {
        return m_link[i].m_dst;
    }

    bool
    SamplesRoutingChannel::IsInitialized(void) const
    {
        NS_ASSERT(m_link[0].m_state != INITIALIZING);
        NS_ASSERT(m_link[1].m_state != INITIALIZING);
        return true;
    }

} // namespace ns3
