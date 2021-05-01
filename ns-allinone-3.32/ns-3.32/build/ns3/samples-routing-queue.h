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

#ifndef SAMPLES_ROUTING_QUEUE_H
#define SAMPLES_ROUTING_QUEUE_H

#include "samples-routing-channel.h"
#include "samples-routing-packet.h"
#include "samples-routing-net-device.h"
#include <queue>

namespace ns3
{
    class SamplesRoutingPacket;
    class SamplesRoutingNetDevice;
    class SamplesRoutingChannel;

    class SamplesRoutingQueue : public Object
    {
    public:
        static TypeId GetTypeId(void);
        SamplesRoutingQueue();
        SamplesRoutingQueue(uint32_t capacity);
        ~SamplesRoutingQueue();

        /**
         * \brief Dispose of the object
         */
        virtual void DoDispose(void);

        Ptr<SamplesRoutingPacket> DequeuePkg();
        void InqueuPkg(Ptr<SamplesRoutingPacket> p);

        uint32_t GetQueueLength();

        void SetCapacity(uint32_t capacity);

        void SetDev(Ptr<SamplesRoutingNetDevice> dev);

        Ptr<SamplesRoutingNetDevice> GetDev();

        uint32_t GetDropBytes();

    private:
        std::queue<Ptr<SamplesRoutingPacket>> m_queue;
        Ptr<SamplesRoutingNetDevice> m_dev;
        uint32_t m_frameCapacity; //the queue's capacityI
        uint32_t m_dropBytes;

        TracedCallback<Ptr<SamplesRoutingQueue>, Ptr<SamplesRoutingPacket>, Time> m_queueTimeTrace;
    };
} // namespace ns3

#endif