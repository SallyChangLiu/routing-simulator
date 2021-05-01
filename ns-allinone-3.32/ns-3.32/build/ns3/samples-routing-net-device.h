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

#ifndef SAMPLES_ROUTING_NETDEVICE_H
#define SAMPLES_ROUTING_NETDEVICE_H

#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "samples-routing-queue.h"
#include "samples-routing-packet.h"
#include "samples-routing-channel.h"
#include "samples-routing-node.h"
#include <queue>

namespace ns3
{
    template <typename Item>
    class Queue;
    class SamplesRoutingPacket;
    class SamplesRoutingQueue;
    class SamplesRoutingChannel;
    class SamplesRoutingNode;

    /**
     * \ingroup samples-routing
     * \brief The samples-routing-net-device class specializes the NetDevice abstract
     * base class.
     */

    class SamplesRoutingNetDevice : public NetDevice
    {
    public:
        /**
         * \brief Get the TypeId
         *
         * \return The TypeId for this class
         */
        static TypeId GetTypeId(void);

        /**
         * Construct a SamplesRoutingNetDevice
         *
         * This is the constructor for the SamplesRoutingNetDevice.  It takes as a
         * parameter a pointer to the Node to which this device is connected,
         * as well as an optional DataRate object.
         */
        SamplesRoutingNetDevice();

        /**
         * Destroy a SamplesRoutingNetDevice
         *
         * This is the destructor for the SamplesRoutingNetDevice.
         */
        ~SamplesRoutingNetDevice();

        /**
         * \brief Dispose of the object
         */
        virtual void DoDispose (void);

        /**
         * Attach the device to a channel.
         *
         * \param ch Ptr to the channel to which this object is being attached.
         * \return true if the operation was successful (always true actually)
         */
        bool Attach(Ptr<SamplesRoutingChannel> ch);

        typedef Callback<void,Ptr<SamplesRoutingPacket>> ReceiveCallback;
        void SetRxCallBack(ReceiveCallback cb);

        /**
         * Attach a queue to the SamplesRoutingNetDevice.
         *
         * The SamplesRoutingNetDevice "owns" a queue that implements a queueing
         * method such as DropTailQueue or RedQueue
         *
         * \param queue Ptr to the new queue.
         */
        void SetQueue(Ptr<SamplesRoutingQueue> queue);

        /**
         * Get a copy of the attached Queue.
         *
         * \returns Ptr to the queue.
         */
        Ptr<SamplesRoutingQueue> GetQueue(void) const;


        /**
         * Start transmit process.
         */
        void TransmitStart(Ptr<SamplesRoutingPacket> p);

        void CompleteTransimit();

        /**
         * Receive a packet from a connected Channel.
         *
         * The NetDevice receives packets from its connected channel
         * and forwards them up the protocol stack.  This is the public method
         * used by the channel to indicate that the last bit of a packet has
         * arrived at the device.
         *
         * \param p Ptr to the received packet.
         */
        void Receive(Ptr<SamplesRoutingPacket> p);

        virtual void SetReceiveCallback(NetDevice::ReceiveCallback cb);

        // The remaining methods are documented in ns3::NetDevice*

        virtual void SetIfIndex(const uint32_t index);
        virtual uint32_t GetIfIndex(void) const;

        virtual Ptr<Channel> GetChannel(void) const;

        virtual void SetAddress(Address address);
        virtual Address GetAddress(void) const;

        virtual bool SetMtu(const uint16_t mtu);
        virtual uint16_t GetMtu(void) const;

        virtual bool IsLinkUp(void) const;

        virtual void AddLinkChangeCallback(Callback<void> callback);

        virtual bool IsBroadcast(void) const;
        virtual Address GetBroadcast(void) const;

        virtual bool IsMulticast(void) const;
        virtual Address GetMulticast(Ipv4Address multicastGroup) const;

        virtual bool IsPointToPoint(void) const;
        virtual bool IsBridge(void) const;

        virtual bool Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber);
        virtual bool SendFrom(Ptr<Packet> packet, const Address &source, const Address &dest,
                              uint16_t protocolNumber);

        Ptr<SamplesRoutingNode> GetSNode(void);
        void SetSNode(Ptr<SamplesRoutingNode> node);

        virtual Ptr<Node> GetNode(void) const;
        virtual void SetNode(Ptr<Node> node);

        virtual bool NeedsArp(void) const;

        virtual Address GetMulticast(Ipv6Address addr) const;

        virtual void SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb);
        virtual bool SupportsSendFrom(void) const;

        DataRate GetDataRate() const;

        void SetDataRate(DataRate rate);

        /**
         * \returns the address of the remote device connected to this device
         * through the Dpsk channel.
         */
        Address GetRemote (void) const;

        uint32_t GetRxBytes() const;
        uint32_t GetTxBytes() const;


    private:
        Ptr<SamplesRoutingQueue> m_queue;
        Ptr<SamplesRoutingChannel> m_channel;
        Ptr<SamplesRoutingNode> m_node;
        DataRate m_rate;
        Mac48Address m_address; //!< Mac48Address of this NetDevice
        bool m_isBusy;
        bool m_isLinkUp;
        ReceiveCallback m_receiveCallback;
        uint32_t m_txBytes;
        uint32_t m_rxBytes;

        NetDevice::ReceiveCallback m_rxCallback; //!< Receive callback
        //some values
        uint32_t m_ifIndex; //!< Index of the interface
        NetDevice::PromiscReceiveCallback m_promiscCallback; //!< Receive callback (promisc data)
        TracedCallback<> m_linkChangeCallbacks; //!< Callback for the link change event
    };

} // namespace ns3

#endif