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
	#undef __CLIENT_INTERFACE_H__
#endif


#ifndef __CLIENT_INTERFACE_H__
#define __CLIENT_INTERFACE_H__

// common include	
#if defined(CLIENT)
#include "clientapp.hpp"
#endif
#include "client_interface_macros.hpp"
#include "network/interface_defs.hpp"
#include "server/mercury_errors.hpp"
#include "common.hpp"

//#define NDEBUG
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace KBEngine{

/**
	CLIENT所有消息接口在此定义
*/
NETWORK_INTERFACE_DECLARE_BEGIN(ClientInterface)
	// 创建账号失败。
	CLIENT_MESSAGE_DECLARE_ARGS1(onCreateAccountResult,			MERCURY_FIXED_MESSAGE,
									MERCURY_ERROR_CODE,			failedcode)

	// 登录成功。
	CLIENT_MESSAGE_DECLARE_STREAM(onLoginSuccessfully,			MERCURY_VARIABLE_MESSAGE)

	// 登录失败。
	CLIENT_MESSAGE_DECLARE_ARGS1(onLoginFailed,					MERCURY_FIXED_MESSAGE,
									MERCURY_ERROR_CODE,			failedcode)

	// 服务器端已经创建了一个与客户端关联的代理Entity || 登录网关成功。
	CLIENT_MESSAGE_DECLARE_ARGS3(onCreatedProxies,				MERCURY_VARIABLE_MESSAGE,
									uint64,						rndUUID,
									ENTITY_ID,					eid,
									std::string,				entityType)

	// 服务器端已经创建了一个Entity。
	CLIENT_MESSAGE_DECLARE_ARGS2(onCreatedEntity,				MERCURY_VARIABLE_MESSAGE,
									ENTITY_ID,					eid,
									std::string,				entityType)

	// 登录网关失败。
	CLIENT_MESSAGE_DECLARE_ARGS1(onLoginGatewayFailed,			MERCURY_FIXED_MESSAGE,
									MERCURY_ERROR_CODE,			failedcode)

	// 服务器上的entity已经有了一个cell部分。
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityGetCell,				MERCURY_FIXED_MESSAGE,
									ENTITY_ID,					eid)

	// 服务器上的entity已经进入游戏世界了。
	CLIENT_MESSAGE_DECLARE_ARGS2(onEntityEnterWorld,			MERCURY_FIXED_MESSAGE,
									ENTITY_ID,					eid,
									SPACE_ID,					spaceID)

	// 服务器上的entity已经离开游戏世界了。
	CLIENT_MESSAGE_DECLARE_ARGS2(onEntityLeaveWorld,			MERCURY_FIXED_MESSAGE,
									ENTITY_ID,					eid,
									SPACE_ID,					spaceID)

	// 告诉客户端某个entity销毁了， 此类entity通常是还未onEntityEnterWorld。
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityDestroyed,				MERCURY_FIXED_MESSAGE,
									ENTITY_ID,					eid)

	// 服务器上的entity已经进入space了。
	CLIENT_MESSAGE_DECLARE_ARGS2(onEntityEnterSpace,			MERCURY_FIXED_MESSAGE,
									ENTITY_ID,					eid,
									SPACE_ID,					spaceID)

	// 服务器上的entity已经离开space了。
	CLIENT_MESSAGE_DECLARE_ARGS2(onEntityLeaveSpace,			MERCURY_FIXED_MESSAGE,
									ENTITY_ID,					eid,
									SPACE_ID,					spaceID)

	// 远程呼叫entity方法
	CLIENT_MESSAGE_DECLARE_STREAM(onRemoteMethodCall,			MERCURY_VARIABLE_MESSAGE)

	// 服务器更新entity属性
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdatePropertys,			MERCURY_VARIABLE_MESSAGE)

NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
