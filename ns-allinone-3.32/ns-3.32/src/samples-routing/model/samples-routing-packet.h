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

#ifndef SAMPLES_ROUTING_PACKET_H
#define SAMPLES_ROUTING_PACKET_H

#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/simulator.h"
#include "ns3/network-module.h"
#include "ns3/packet.h"
#include <string>

namespace ns3
{
  /**
     * \ingroup samples-routing
     * \class SamplesRoutingPacket
     * \brief packet.
     */

  class SamplesRoutingPacket : public Object
  {
  public:
    static TypeId GetTypeId(void);
    SamplesRoutingPacket();

    /**
          * constractor
          * \param psize packet size
          * \param sIP source IP
          * \param dIP destination IP
          */
    SamplesRoutingPacket(uint32_t psize, Ipv4Address sIP, Ipv4Address dIP);

    /**
     * \brief Dispose of the object
     */
    virtual void DoDispose(void);

    /**
     * set packet size
     * \param psize the packet size.
     */
    void SetSize(uint32_t psiz);

    /**
     * get the packet size--bytes
     * \return the packet size.
     */
    uint32_t GetSize();

    /**
     * set the source Ipv4 address of the packet
     * \param sIp the source Ipv4 address of the packet.
     */
    void SetSourceIp(Ipv4Address sIp);

    /**
     * get the source ip address of the packet
     * \return the source ipv4 address of the packet.
     */
    Ipv4Address GetSourceIp();

    /**
     * set the destination ipv4 address of the packet
     * \param dIp the destination address of the packet.
     */
    void SetDstIp(Ipv4Address dIp);

    /**
     * get the destination ipv4 address of the packet
     * \return the destination ipv4 address of the packet.
     */
    Ipv4Address GetDstIp();

    /**
     * set the hop number that the packet has gone through
     * \param c the hop number.
     */
    void SetHopCount(uint32_t c);

    /**
     * get the hop number that the packet has gone through
     * \return the hop number that the packet has gone through.
     */
    uint32_t GetHopCount();

    /**
     * set the cerate time for the packet
     * \param t the cerate time for the packet.
     */
    void SetCreateTime(Time t);

    /**
     * get cerate time of the packet
     * \return cerate time of the packet.
     */
    Time GetCreateTime();

    /**
     * set the in queue time of the packet
     * \param t the in queue time of the packet.
     */
    void SetInTime(Time t);

    /**
     * get the in queue time of the packet
     * \return the in queue time of the packet.
     */
    Time GetInTime();

    void SetName(std::string name);

    std::string GetName();

  private:
    uint32_t m_size;
    Ipv4Address m_sIp, m_dIp; //!< source IP, dst IP
    uint32_t m_hopCount;
    Time m_createTime;
    Time m_inTime; //packet inqueue time stamp
    std::string m_name;
  };
} // namespace ns3
#endif