/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "udp_packet.hpp"
#ifndef CODE_INLINE
#include "udp_packet.ipp"
#endif
#include "network/bundle.hpp"
#include "network/endpoint.hpp"
#include "network/network_interface.hpp"
#include "network/message_handler.hpp"

namespace KBEngine { 
namespace Mercury
{
//-------------------------------------------------------------------------------------
static ObjectPool<UDPPacket> _g_objPool;
ObjectPool<UDPPacket>& UDPPacket::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
UDPPacket::UDPPacket(MessageID msgID, size_t res):
Packet(msgID, false, res)
{
	data_resize(PACKET_MAX_SIZE_UDP);
	wpos(0);
}

//-------------------------------------------------------------------------------------
UDPPacket::~UDPPacket(void)
{
}

//-------------------------------------------------------------------------------------
int UDPPacket::recvFromEndPoint(EndPoint & ep, Address* pAddr)
{
	int len = ep.recvfrom(data(), PACKET_MAX_SIZE_UDP,
		(u_int16_t*)&pAddr->port, (u_int32_t*)&pAddr->ip);

	wpos(len);
	return len;
}

//-------------------------------------------------------------------------------------
}
}
