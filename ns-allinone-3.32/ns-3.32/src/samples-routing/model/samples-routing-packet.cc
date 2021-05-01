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

#include "samples-routing-packet.h"
#include "ns3/log.h"

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("SamplesRoutingPacket");
    NS_OBJECT_ENSURE_REGISTERED(SamplesRoutingPacket);

    TypeId
    SamplesRoutingPacket::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::SamplesRoutingPacket")
                                .SetParent<Object>()
                                .SetGroupName("samples-routing")
                                .AddConstructor<SamplesRoutingPacket>();
        return tid;
    }

    SamplesRoutingPacket::SamplesRoutingPacket()
    {
        NS_LOG_FUNCTION_NOARGS();
    }

    SamplesRoutingPacket::SamplesRoutingPacket(uint32_t psize, Ipv4Address sIP, Ipv4Address dIP)
        : m_size(psize),
          m_sIp(sIP),
          m_dIp(dIP),
          m_hopCount(0),
          m_createTime(Time(0))
    {
        NS_LOG_FUNCTION(this << psize << sIP << dIP);
    }

    void SamplesRoutingPacket::DoDispose()
    {
        NS_LOG_FUNCTION(this);
        m_createTime = Time(0);
        m_hopCount = 0;
        m_inTime = Time(0);
        m_size = 0;
        Object::DoDispose();
    }

    void SamplesRoutingPacket::SetSize(uint32_t psiz)
    {
        NS_LOG_FUNCTION(psiz);

        m_size = psiz;
    }

    uint32_t SamplesRoutingPacket::GetSize()
    {
        return m_size;
    }

    void SamplesRoutingPacket::SetSourceIp(Ipv4Address sIp)
    {
        NS_LOG_FUNCTION(sIp);

        m_sIp = sIp;
    }

    Ipv4Address SamplesRoutingPacket::GetSourceIp()
    {
        return m_sIp;
    }

    void SamplesRoutingPacket::SetDstIp(Ipv4Address dIp)
    {
        NS_LOG_FUNCTION(dIp);

        m_dIp = dIp;
    }

    Ipv4Address SamplesRoutingPacket::GetDstIp()
    {
        return m_dIp;
    }

    void SamplesRoutingPacket::SetHopCount(uint32_t c)
    {
        NS_LOG_FUNCTION(c);

        m_hopCount = c;
    }

    uint32_t SamplesRoutingPacket::GetHopCount()
    {
        return m_hopCount;
    }

    void SamplesRoutingPacket::SetCreateTime(Time t)
    {
        NS_LOG_FUNCTION(t);

        m_createTime = t;
    }

    Time SamplesRoutingPacket::GetCreateTime()
    {
        return m_createTime;
    }

    void SamplesRoutingPacket::SetInTime(Time t)
    {
        NS_LOG_FUNCTION(t);

        m_inTime = t;
    }

    Time SamplesRoutingPacket::GetInTime()
    {
        return m_inTime;
    }

    void SamplesRoutingPacket::SetName(std::string name)
    {
        NS_LOG_FUNCTION(name);
        m_name = name;
    }

    std::string SamplesRoutingPacket::GetName()
    {
        return m_name;
    }
} // namespace ns3
