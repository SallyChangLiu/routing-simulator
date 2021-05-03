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

#ifndef SAMPLES_ROUTING_APPLICATION_H
#define SAMPLES_ROUTING_APPLICATION_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include "ns3/core-module.h"
#include "samples-routing-net-device.h"
#include "samples-routing-router.h"
#include "samples-routing-node.h"
#include "samples-routing.h"
#include <vector>

namespace ns3
{
    //template <typename Item>
    class SamplesRoutingRouter;
    class SamplesRoutingPacket;
    class SamplesRoutingNode;

    /**
     * \ingroup samples-routing
     * \brief The samples-routing-app class is used for generating pacekts
     */

    class SamplesRoutingApp : public Application
    {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId(void);

        SamplesRoutingApp();
        virtual ~SamplesRoutingApp();

        void SetSendInterval(Time t);

        void SetNode(Ptr<SamplesRoutingNode> nd);

        void SetupDestAddr(Ipv4Address dstaddr);

        /**
         * receive a packet.
         * \param p the packet need to be received
         */
        void HandleRx(Ptr<SamplesRoutingPacket> p);

        /**
         * \brief Application specific startup code
         *
         * The StartApplication method is called at the start time specified by Start
         * This method should be overridden by all or most application
         * subclasses.
         */
        void StartApplication();

        /**
         * \brief Application specific shutdown code
         *
         * The StopApplication method is called at the stop time specified by Stop
         * This method should be overridden by all or most application
         * subclasses.
         */
        void StopApplication(void);

    protected:
        virtual void DoDispose(void);
        virtual void DoInitialize(void);

    private:
        /**
         * The trace source fired for Receiving a packet.
         *
         * \param Ptr pointer of packet
         */
        TracedCallback<Ptr<SamplesRoutingPacket>> m_packetRxCompleteTrace;

        std::vector<Ipv4Address> m_destAddress; //vector for pacekt destIP
        Ptr<SamplesRoutingNode> m_node;
        Time m_sentInterval;
    };

} // namespace ns3

#endif /* SAMPLES_ROUTING_APPLICATION_H */