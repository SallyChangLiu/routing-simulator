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

#ifndef SAMPLES_ROUTING_CHANNEL_H
#define SAMPLES_ROUTING_CHANNEL_H

#include "samples-routing-net-device.h"
#include "samples-routing-packet.h"
#include "ns3/channel.h"

namespace ns3
{
    class SamplesRoutingNetDevice;
    class SamplesRoutingPacket;
    class SamplesRoutingChannel : public Channel
    {
    public:

        /**
         * \brief Get the TypeId
         *
         * \return The TypeId for this class
         */
        static TypeId GetTypeId (void);

        /**
         * \brief Create a DpskChannel
         *
         * By default, you get a channel that has an "infinitely" fast
         * transmission speed and zero delay.
         */
        SamplesRoutingChannel ();

        /**
         * \brief Dispose of the object
         */
        virtual void DoDispose (void);

        /**
         * \brief Attach a given queue to this channel
         * \param queue pointer to the queue to attach to the channel
         */
        void Attach(Ptr<SamplesRoutingNetDevice> device);

        /**
         * \brief Transmit a packet over this channel
         * \param p Packet to transmit
         * \param src Source queue
         * \param txTime Transmit time to apply
         * \returns true if successful (currently always true)
         */
        bool TransmitStart(Ptr<SamplesRoutingPacket> p, Ptr<SamplesRoutingNetDevice> src, Time txTime);

        /**
         * \brief Get number of devices on this channel
         * \returns number of devices on this channel
         */
        virtual std::size_t GetNDevices (void) const;

        /**
         * \brief Get DpskNetDevice corresponding to index i on this channel
         * \param i Index number of the device requested
         * \returns Ptr to DpskNetDevice requested
         */
        Ptr<SamplesRoutingNetDevice> GetSamplesRoutingNetDevice (std::size_t i) const;

        /**
         * \brief Get NetDevice corresponding to index i on this channel
         * \param i Index number of the device requested
         * \returns Ptr to NetDevice requested
         */
        virtual Ptr<NetDevice> GetDevice (std::size_t i) const;

        void SetDelay(Time dly);

    protected:
        /**
         * \brief Get the delay associated with this channel
         * \returns Time delay
         */
        Time GetDelay(void) const;

        

        /**
         * \brief Check to make sure the link is initialized
         * \returns true if initialized, asserts otherwise
         */
        bool IsInitialized (void) const;

        /**
         * \brief Get the net-device source
         * \param i the link requested
         * \returns Ptr to DpskNetDevice source for the
         * specified link
         */
        Ptr<SamplesRoutingNetDevice> GetSource (uint32_t i) const;

        /**
         * \brief Get the net-device destination
         * \param i the link requested
         * \returns Ptr to DpskNetDevice destination for
         * the specified link
         */
        Ptr<SamplesRoutingNetDevice> GetDestination (uint32_t i) const;

    private:
        static const std::size_t N_DEVICES = 2;
        Time m_delay;          //!< Propagation delay
        std::size_t m_nDevices; //!< devices of this channel

        /** \brief Wire states
         *
         */
        enum WireState
        {
            /** Initializing state */
            INITIALIZING,
            /** Idle state (no transmission from NetDevice) */
            IDLE,
            /** Transmitting state (data being transmitted from NetDevice. */
            TRANSMITTING,
            /** Propagating state (data is being propagated in the channel. */
            PROPAGATING
        };

        /**
         * \brief Wire model for the DpskChannel
         */
        class Link
        {
        public:
            /** \brief Create the link, it will be in INITIALIZING state
             *
             */
            Link() : m_state(INITIALIZING), m_src(0), m_dst(0)
            {
            }

            WireState m_state;              //!< State of the link
            Ptr<SamplesRoutingNetDevice> m_src; //!< First NetDevice
            Ptr<SamplesRoutingNetDevice> m_dst; //!< Second NetDevice
        };

        Link m_link[N_DEVICES]; //!< Link model
    };
} // namespace ns3

#endif