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
#include "ns3/queue.h"
#include "ns3/simulator.h"
#include "ns3/mac48-address.h"
#include "ns3/llc-snap-header.h"
#include "ns3/error-model.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/object.h"
#include "samples-routing-channel.h"
#include "samples-routing-packet.h"
#include "samples-routing-net-device.h"
#include "samples-routing-queue.h"

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("SamplesRoutingNetDevice");
    NS_OBJECT_ENSURE_REGISTERED(SamplesRoutingNetDevice);

    TypeId
    SamplesRoutingNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::SamplesRoutingNetDevice")
                                .SetParent<NetDevice>()
                                .SetGroupName("samples-routing")
                                .AddConstructor<SamplesRoutingNetDevice>()
                                .AddAttribute("Address", "The MAC address of this device.", Mac48AddressValue(Mac48Address("ff:ff:ff:ff:ff:ff")),
                                              MakeMac48AddressAccessor(&SamplesRoutingNetDevice::m_address),
                                              MakeMac48AddressChecker())
                                .AddAttribute("DataRate", "The default data rate for point to point links",
                                              DataRateValue(DataRate("32768b/s")),
                                              MakeDataRateAccessor(&SamplesRoutingNetDevice::m_rate), MakeDataRateChecker());

        return tid;
    }

    SamplesRoutingNetDevice::SamplesRoutingNetDevice()
    {
        NS_LOG_FUNCTION(this);
        m_isLinkUp = false;
        m_isBusy = false;
        m_channel = 0;
        m_txBytes = 0;
        m_rxBytes = 0;
    }

    SamplesRoutingNetDevice::~SamplesRoutingNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    void SamplesRoutingNetDevice::DoDispose()
    {
        NS_LOG_FUNCTION(this);
        m_node = 0;
        m_queue = 0;
        NetDevice::DoDispose();
    }

    bool SamplesRoutingNetDevice::Attach(Ptr<SamplesRoutingChannel> ch)
    {
        NS_LOG_FUNCTION(this << &ch);

        m_channel = ch;
        m_isLinkUp = true;
        m_channel->Attach(this);
        return true;
    }

    void
    SamplesRoutingNetDevice::SetQueue(Ptr<SamplesRoutingQueue> q)
    {
        NS_LOG_FUNCTION(this << q);
        m_queue = q;
        m_queue->SetDev(this);
    }

    Ptr<SamplesRoutingQueue>
    SamplesRoutingNetDevice::GetQueue(void) const
    {
        NS_LOG_FUNCTION(this);
        return m_queue;
    }

    void SamplesRoutingNetDevice::TransmitStart(Ptr<SamplesRoutingPacket> p)
    {
        NS_LOG_FUNCTION(this << p);
        if (m_isBusy == false) //we are not busy can send the packet right now
        {
            m_isBusy = true;
            Time txTime = m_rate.CalculateBytesTxTime(p->GetSize());

            Simulator::Schedule(txTime, &SamplesRoutingNetDevice::CompleteTransimit, this);

            m_txBytes += p->GetSize();
            m_channel->TransmitStart(p, this, txTime);
        }
        else //we are busy now, so just inqueue this packet
        {
            m_queue->InqueuPkg(p);
        }
    }

    void SamplesRoutingNetDevice::CompleteTransimit()
    {
        m_isBusy = false;

        if (m_queue->GetQueueLength() > 0)
        {
            Ptr<SamplesRoutingPacket> p = m_queue->DequeuePkg();
            if (p != 0)
                TransmitStart(p);
        }
    }

    void SamplesRoutingNetDevice::Receive(Ptr<SamplesRoutingPacket> p)
    {
        NS_LOG_FUNCTION(this << p);

        m_rxBytes += p->GetSize();
        //m_rxCallback(this,p,0,GetRemote()) // Other protocols
        m_receiveCallback(p);
    }

    void
    SamplesRoutingNetDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb)
    {
        m_rxCallback = cb;
    }

    void SamplesRoutingNetDevice::SetDataRate(DataRate rate)
    {
        NS_LOG_FUNCTION(this << rate);
        m_rate = rate;
    }

    DataRate SamplesRoutingNetDevice::GetDataRate() const
    {
        return m_rate;
    }

    uint32_t
    SamplesRoutingNetDevice::GetIfIndex(void) const
    {
        return m_ifIndex;
    }

    void SamplesRoutingNetDevice::SetIfIndex(const uint32_t index)
    {
        NS_LOG_FUNCTION(this);
        m_ifIndex = index;
    }

    Ptr<Channel>
    SamplesRoutingNetDevice::GetChannel(void) const
    {
        return m_channel;
    }

    Address
    SamplesRoutingNetDevice::GetAddress(void) const
    {
        return m_address;
    }

    void SamplesRoutingNetDevice::SetAddress(Address address)
    {
        NS_LOG_FUNCTION(this << address);
        m_address = Mac48Address::ConvertFrom(address);
    }

    bool
    SamplesRoutingNetDevice::IsLinkUp(void) const
    {
        NS_LOG_FUNCTION(this);
        return m_isLinkUp;
    }

    void SamplesRoutingNetDevice::AddLinkChangeCallback(Callback<void> callback)
    {
        NS_LOG_FUNCTION(this);
        m_linkChangeCallbacks.ConnectWithoutContext(callback);
    }

    bool
    SamplesRoutingNetDevice::IsBroadcast(void) const
    {
        NS_LOG_FUNCTION(this);
        return true;
    }

    Address
    SamplesRoutingNetDevice::GetBroadcast(void) const
    {
        NS_LOG_FUNCTION(this);
        return Mac48Address("ff:ff:ff:ff:ff:ff");
    }

    bool
    SamplesRoutingNetDevice::IsMulticast(void) const
    {
        NS_LOG_FUNCTION(this);
        return true;
    }

    Address
    SamplesRoutingNetDevice::GetMulticast(Ipv4Address multicastGroup) const
    {
        NS_LOG_FUNCTION(this);
        return Mac48Address("01:00:5e:00:00:00");
    }

    Address
    SamplesRoutingNetDevice::GetMulticast(Ipv6Address addr) const
    {
        NS_LOG_FUNCTION(this << addr);
        return Mac48Address("33:33:00:00:00:00");
    }

    bool
    SamplesRoutingNetDevice::IsPointToPoint(void) const
    {
        NS_LOG_FUNCTION(this);
        return true;
    }

    bool
    SamplesRoutingNetDevice::IsBridge(void) const
    {
        NS_LOG_FUNCTION(this);
        return false;
    }

    bool SamplesRoutingNetDevice::Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
    {
        NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
        return SendFrom(packet, m_address, dest, protocolNumber);
    }

    bool SamplesRoutingNetDevice::SendFrom(Ptr<Packet> packet, const Address &source, const Address &dest,
                                           uint16_t protocolNumber)
    {
        //this function is not used by now
        return false;
    }

    void SamplesRoutingNetDevice::SetSNode(Ptr<SamplesRoutingNode> node)
    {
        NS_LOG_FUNCTION(this);
        m_node = node;
    }

    Ptr<SamplesRoutingNode>
    SamplesRoutingNetDevice::GetSNode(void)
    {
        return m_node;
    }

    void SamplesRoutingNetDevice::SetNode(Ptr<Node> node)
    {
    }

    Ptr<Node>
    SamplesRoutingNetDevice::GetNode(void) const
    {
        return NULL;
    }

    bool SamplesRoutingNetDevice::NeedsArp(void) const
    {
        NS_LOG_FUNCTION(this);
        return false;
    }

    void SamplesRoutingNetDevice::SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb)
    {
        m_promiscCallback = cb;
    }

    bool
    SamplesRoutingNetDevice::SupportsSendFrom(void) const
    {
        NS_LOG_FUNCTION(this);
        return true;
    }

    Address
    SamplesRoutingNetDevice::GetRemote(void) const
    {
        NS_LOG_FUNCTION(this);
        NS_ASSERT(m_channel->GetNDevices() == 2);
        for (std::size_t i = 0; i < m_channel->GetNDevices(); ++i)
        {
            Ptr<NetDevice> tmp = m_channel->GetDevice(i);
            if (tmp != this)
            {
                return tmp->GetAddress();
            }
        }
        NS_ASSERT(false);
        // quiet compiler.
        return Address();
    }

    bool SamplesRoutingNetDevice::SetMtu(const uint16_t mtu)
    {
        return true;
    }

    uint16_t
    SamplesRoutingNetDevice::GetMtu(void) const
    {
        NS_LOG_FUNCTION(this);
        return 0;
    }

    void SamplesRoutingNetDevice::SetRxCallBack(ReceiveCallback cb)
    {
        m_receiveCallback = cb;
    }

    uint32_t SamplesRoutingNetDevice::GetRxBytes() const
    {
        return m_rxBytes;
    }
    uint32_t SamplesRoutingNetDevice::GetTxBytes() const
    {
        return m_txBytes;
    }

} // namespace ns3