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


#include "baseappmgr.hpp"
#include "baseappmgr_interface.hpp"
#include "network/common.hpp"
#include "network/tcp_packet.hpp"
#include "network/udp_packet.hpp"
#include "network/message_handler.hpp"
#include "thread/threadpool.hpp"
#include "server/componentbridge.hpp"

#include "../../server/cellappmgr/cellappmgr_interface.hpp"
#include "../../server/baseapp/baseapp_interface.hpp"
#include "../../server/cellapp/cellapp_interface.hpp"
#include "../../server/dbmgr/dbmgr_interface.hpp"
#include "../../server/loginapp/loginapp_interface.hpp"

namespace KBEngine{
	
ServerConfig g_serverConfig;
KBE_SINGLETON_INIT(Baseappmgr);

//-------------------------------------------------------------------------------------
Baseappmgr::Baseappmgr(Mercury::EventDispatcher& dispatcher, 
			 Mercury::NetworkInterface& ninterface, 
			 COMPONENT_TYPE componentType,
			 COMPONENT_ID componentID):
	ServerApp(dispatcher, ninterface, componentType, componentID),
	gameTimer_(),
	forward_baseapp_messagebuffer_(ninterface, BASEAPP_TYPE)
{
}

//-------------------------------------------------------------------------------------
Baseappmgr::~Baseappmgr()
{
}

//-------------------------------------------------------------------------------------
bool Baseappmgr::run()
{
	return ServerApp::run();
}

//-------------------------------------------------------------------------------------
void Baseappmgr::handleTimeout(TimerHandle handle, void * arg)
{
	switch (reinterpret_cast<uintptr>(arg))
	{
		case TIMEOUT_GAME_TICK:
			this->handleGameTick();
			break;
		default:
			break;
	}

	ServerApp::handleTimeout(handle, arg);
}

//-------------------------------------------------------------------------------------
void Baseappmgr::handleGameTick()
{
	 //time_t t = ::time(NULL);
	 //DEBUG_MSG("CellApp::handleGameTick[%"PRTime"]:%u\n", t, time_);
	
	time_++;
	getNetworkInterface().handleChannels(&BaseappmgrInterface::messageHandlers);
}

//-------------------------------------------------------------------------------------
bool Baseappmgr::initializeBegin()
{
	return true;
}

//-------------------------------------------------------------------------------------
bool Baseappmgr::inInitialize()
{
	return true;
}

//-------------------------------------------------------------------------------------
bool Baseappmgr::initializeEnd()
{
	gameTimer_ = this->getMainDispatcher().addTimer(1000000 / g_kbeSrvConfig.gameUpdateHertz(), this,
							reinterpret_cast<void *>(TIMEOUT_GAME_TICK));
	return true;
}

//-------------------------------------------------------------------------------------
void Baseappmgr::finalise()
{
	gameTimer_.cancel();
	ServerApp::finalise();
}

//-------------------------------------------------------------------------------------
void Baseappmgr::forwardMessage(Mercury::Channel* pChannel, MemoryStream& s)
{
	COMPONENT_ID sender_componentID, forward_componentID;

	s >> sender_componentID >> forward_componentID;
	Components::ComponentInfos* cinfos = Components::getSingleton().findComponent(forward_componentID);
	KBE_ASSERT(cinfos != NULL && cinfos->pChannel != NULL);

	Mercury::Bundle bundle;
	bundle.append((char*)s.data() + s.rpos(), s.opsize());
	bundle.send(this->getNetworkInterface(), cinfos->pChannel);
	s.read_skip(s.opsize());
}

//-------------------------------------------------------------------------------------
void Baseappmgr::reqCreateBaseAnywhere(Mercury::Channel* pChannel, MemoryStream& s) 
{
	Components::COMPONENTS& components = Components::getSingleton().getComponents(BASEAPP_TYPE);
	size_t componentSize = components.size();
	if(componentSize == 0)
	{
		ForwardItem* pFI = new ForwardItem();
		pFI->bundle.newMessage(BaseappInterface::onCreateBaseAnywhere);
		pFI->bundle.append((char*)s.data() + s.rpos(), s.opsize());
		s.read_skip(s.opsize());

		WARNING_MSG("Baseappmgr::reqCreateBaseAnywhere: not found baseapp, message is buffered.\n");
		pFI->pHandler = NULL;
		forward_baseapp_messagebuffer_.push(pFI);
		return;
	}
	
	static uint32 currentBaseappIndex = 0;
	if(currentBaseappIndex > componentSize - 1)
		currentBaseappIndex = 0;
	
	//DEBUG_MSG("Baseappmgr::reqCreateBaseAnywhere: %s opsize=%d, selBaseappIdx=%d.\n", 
	//	pChannel->c_str(), s.opsize(), currentBaseappIndex);

	Components::COMPONENTS::iterator iter = components.begin();
	std::advance(iter, currentBaseappIndex++);
	Mercury::Channel* lpChannel = (*iter).pChannel;

	Mercury::Bundle bundle;
	bundle.newMessage(BaseappInterface::onCreateBaseAnywhere);

	bundle.append((char*)s.data() + s.rpos(), s.opsize());
	bundle.send(this->getNetworkInterface(), lpChannel);
	s.read_skip(s.opsize());
}

//-------------------------------------------------------------------------------------
void Baseappmgr::registerPendingAccountToBaseapp(Mercury::Channel* pChannel, 
							  std::string& accountName, std::string& password)
{
	Components::COMPONENTS& components = Components::getSingleton().getComponents(BASEAPP_TYPE);
	size_t componentSize = components.size();
	if(componentSize == 0)
	{
		ForwardItem* pFI = new ForwardItem();
		pFI->bundle.newMessage(BaseappInterface::registerPendingLogin);
		pFI->bundle << accountName << password;

		WARNING_MSG("Baseappmgr::registerAccountToBaseapp: not found baseapp, message is buffered.\n");
		pFI->pHandler = NULL;
		forward_baseapp_messagebuffer_.push(pFI);
		return;
	}

	static uint32 currentBaseappIndex = 0;
	if(currentBaseappIndex > componentSize - 1)
		currentBaseappIndex = 0;

	DEBUG_MSG("Baseappmgr::registerAccountToBaseapp:%s. allocBaseapp=%d.\n", accountName.c_str(), currentBaseappIndex);

	Components::COMPONENTS::iterator iter = components.begin();
	std::advance(iter, currentBaseappIndex++);
	Mercury::Channel* lpChannel = (*iter).pChannel;

	Mercury::Bundle bundle;
	bundle.newMessage(BaseappInterface::registerPendingLogin);
	ENTITY_ID eid = 0;
	bundle << accountName << password << eid;
	bundle.send(this->getNetworkInterface(), lpChannel);
}

//-------------------------------------------------------------------------------------
void Baseappmgr::registerPendingAccountToBaseappAddr(Mercury::Channel* pChannel, COMPONENT_ID componentID,
								std::string& accountName, std::string& password, ENTITY_ID entityID)
{
	DEBUG_MSG("Baseappmgr::registerPendingAccountToBaseappAddr:%s, componentID=%"PRAppID", entityID=%d.\n", 
		accountName.c_str(), componentID, entityID);

	Components::ComponentInfos* cinfos = Components::getSingleton().findComponent(componentID);
	if(cinfos == NULL || cinfos->pChannel == NULL)
	{
		return;
	}

	Mercury::Bundle bundle;
	bundle.newMessage(BaseappInterface::registerPendingLogin);
	bundle << accountName << password << entityID;
	bundle.send(this->getNetworkInterface(), cinfos->pChannel);
}

//-------------------------------------------------------------------------------------
void Baseappmgr::onPendingAccountGetBaseappAddr(Mercury::Channel* pChannel, 
							  std::string& accountName, uint32 addr, uint16 port)
{
	Components::COMPONENTS& components = Components::getSingleton().getComponents(LOGINAPP_TYPE);
	size_t componentSize = components.size();
	KBE_ASSERT(componentSize > 0);
	Components::COMPONENTS::iterator iter = components.begin();
	Mercury::Channel* lpChannel = (*iter).pChannel;

	Mercury::Bundle bundleToLoginapp;
	bundleToLoginapp.newMessage(LoginappInterface::onLoginAccountQueryBaseappAddrFromBaseappmgr);
	LoginappInterface::onLoginAccountQueryBaseappAddrFromBaseappmgrArgs3::staticAddToBundle(bundleToLoginapp, accountName, addr, port);
	bundleToLoginapp.send(this->getNetworkInterface(), lpChannel);
}

//-------------------------------------------------------------------------------------

}
