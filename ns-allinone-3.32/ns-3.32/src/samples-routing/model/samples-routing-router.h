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

#ifndef SAMPLES_ROUTING_ROUTER_H
#define SAMPLES_ROUTING_ROUTER_H

#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/node.h"
#include "ns3/callback.h"
#include "samples-routing-net-device.h"
#include "samples-routing-packet.h"
#include "samples-routing-node.h"
#include <vector>
#include <map>

namespace ns3
{
    //template <typename Item>
    class SamplesRoutingNetDevice;
    class SamplesRoutingApp;
    class SamplesRoutingPacket;
    class SamplesRoutingNode;
    class SamplesRoutingRouter : public Object
    {
    public:
        /**
         * \brief Get the TypeId
         *
         * \return The TypeId for this class
         */
        static TypeId GetTypeId(void);

        /**
         * Construct a SamplesRoutingRouter
         *
         * This is the constructor for the SamplesRoutingRouter.  It takes as a
         * parameter a pointer to the Node to which this device is connected,
         * as well as an optional DataRate object.
         */
        SamplesRoutingRouter();

        /**
         * Destroy a SamplesRoutingRouter
         *
         * This is the destructor for the SamplesRoutingRouter.
         */
        virtual ~SamplesRoutingRouter();

        /**
         * \brief Dispose of the object
         */
        virtual void DoDispose(void);

        /**
         * send a packet
         * \param p the packet that needs to be sent.
         */
        void HandleMsg(Ptr<SamplesRoutingPacket> p);

        /**
         * this function is used to build a routerTable for a host
         * \param dstIp the destination Ipv4 Address
         * \param outPort the next port to destination IP
         */
        void BuildRouterTable(Ipv4Address dstIp, Ptr<SamplesRoutingNetDevice> outPort);

        typedef Callback<void, Ptr<SamplesRoutingPacket>> RxCallBack; // call back type of receiving

        /**
         * this function setup callback for receing a packet & this packet's destination is another host
         * \param cc the func.
         */
        void SetupRxCallBack(RxCallBack cc);

        void SetNode(Ptr<SamplesRoutingNode> nd);

        Ptr<SamplesRoutingNode> GetNode();

        uint32_t GetDropBytes();

    private:
        uint32_t m_dropBytes;                                                           // dropped packet byte length
        std::map<Ipv4Address, std::vector<Ptr<SamplesRoutingNetDevice>>> m_routerTable; //dest ip ---> net device
        Ptr<SamplesRoutingNode> m_node;

        RxCallBack m_rxCallBack; //call app receiver
        TracedCallback<Ptr<SamplesRoutingRouter>, Ptr<SamplesRoutingPacket>, int> m_RouterOutputGateTrace;
    };
} // namespace ns3

#endif