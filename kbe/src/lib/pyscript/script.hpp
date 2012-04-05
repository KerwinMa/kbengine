/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 kbegine Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
*/
#ifndef __KBENGINE_SCRIPT_H__
#define __KBENGINE_SCRIPT_H__
#include "log/debug_helper.hpp"
#include "cstdkbe/cstdkbe.hpp"
#include "cstdkbe/singleton.hpp"
#include "scriptobject.hpp"
#include "scriptstdouterr.hpp"
#include "scriptstdouterrhook.hpp"

namespace KBEngine{ namespace script{

/** �ű�ϵͳ·�� */
#define SCRIPT_PATH												\
					L"../../res/script;"						\
					L"../../res/script/common;"					\
					L"../../res/script/common/DLLs;"			\
					L"../../res/script/common/Lib"

class Script
{						
public:	
	Script();
	virtual ~Script();
	
	/** ��װ��ж�ؽű�ģ�� */
	virtual bool install(wchar_t* pythonHomeDir, std::wstring pyPaths, const char* moduleName);
	virtual bool uninstall(void);
	
	/** ��ȡ�ű�����ģ�� */
	PyObject* getModule(void)const { return m_module_; }

	int Run_SimpleString(std::string command, std::string* retBufferPtr);


	int registerToModule(const char* attrName, PyObject* pyObj);

protected:
	PyObject* 					m_module_;
	ScriptStdOutErr*			m_pyStdouterr_;
	ScriptStdOutErrHook*		m_pyStdouterrHook_;	// �ṩtelnet ִ�нű�������
} ;

}
}
#endif