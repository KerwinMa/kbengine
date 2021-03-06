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


#if defined(DEFINE_IN_INTERFACE)
	#undef __MESSAGELOG_INTERFACE_H__
#endif


#ifndef __MESSAGELOG_INTERFACE_H__
#define __MESSAGELOG_INTERFACE_H__

// common include	
#if defined(MESSAGELOG)
#include "messagelog.hpp"
#endif
#include "messagelog_interface_macros.hpp"
#include "network/interface_defs.hpp"
//#define NDEBUG
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace KBEngine{

/**
	Messagelog所有消息接口在此定义
*/
NETWORK_INTERFACE_DECLARE_BEGIN(MessagelogInterface)
	// 某app注册自己的接口地址到本app
	MESSAGELOG_MESSAGE_DECLARE_ARGS8(onRegisterNewApp,						MERCURY_VARIABLE_MESSAGE,
									int32,									uid, 
									std::string,							username,
									int8,									componentType, 
									uint64,									componentID, 
									uint32,									intaddr, 
									uint16,									intport,
									uint32,									extaddr, 
									uint16,									extport)

	// 某个app向本app告知处于活动状态。
	MESSAGELOG_MESSAGE_DECLARE_ARGS2(onAppActiveTick,						MERCURY_FIXED_MESSAGE,
									COMPONENT_TYPE,							componentType, 
									COMPONENT_ID,							componentID)
NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
