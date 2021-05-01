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

#ifndef SAMPLES_ROUTING_NODE_H
#define SAMPLES_ROUTING_NODE_H

#include "samples-routing-net-device.h"
#include "samples-routing-router.h"
#include "samples-routing-app.h"
#include "ns3/ipv4-address.h"
#include <vector>
#include <string>

namespace ns3
{
    class SamplesRoutingNetDevice;
    class SamplesRoutingRouter;
    class SamplesRoutingApp;

    class SamplesRoutingNode : public Node
    {
    public:
        /**
         * \brief Get the TypeId
         *
         * \return The TypeId for this class
         */
        static TypeId GetTypeId(void);

        /**
         * Construct a SamplesRoutingNode
         *
         * This is the constructor for the SamplesRoutingNode.  It takes as a
         * parameter a pointer to the Node to which this device is connected,
         * as well as an optional DataRate object.
         */
        SamplesRoutingNode();

        SamplesRoutingNode(Ipv4Address addr);

        /**
         * Destroy a SamplesRoutingNode
         *
         * This is the destructor for the SamplesRoutingNode.
         */
        ~SamplesRoutingNode();

        /**
         * \brief Dispose of the object
         */
        virtual void DoDispose(void);

        void SetAddress(Ipv4Address addr);

        Ipv4Address GetAddress();

        void AddRouter(Ptr<SamplesRoutingRouter> r);

        Ptr<SamplesRoutingRouter> GetRouter();

        uint32_t AddApplication(Ptr<SamplesRoutingApp> application);

        uint32_t AddDevice(Ptr<SamplesRoutingNetDevice> device);

        Ptr<SamplesRoutingNetDevice> GetDevice(uint32_t index) const;

        void SetName(std::string name);

        std::string GetName();

        Ptr<SamplesRoutingApp> GetApplication(uint32_t index);

        uint32_t GetAppSize();

        uint32_t GetNApplications() const;
        uint32_t GetNDevices() const;

    private:
        Ipv4Address m_address;
        Ptr<SamplesRoutingRouter> m_router;
        std::vector<Ptr<SamplesRoutingNetDevice>> m_devices; //!< Devices associated to this node
        std::vector<Ptr<SamplesRoutingApp>> m_applications;  //!< Applications associated to this node
        std::string m_name;
    };
} // namespace ns3
#endif