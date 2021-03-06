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

#include "fixeddict.hpp"

namespace KBEngine{ 

/** python map操作所需要的方法表 */
PyMappingMethods FixedDict::mappingMethods =
{
	(lenfunc)FixedDict::mp_length,					// mp_length
	(binaryfunc)FixedDict::mp_subscript,			// mp_subscript
	(objobjargproc)FixedDict::mp_ass_subscript		// mp_ass_subscript
};


SCRIPT_METHOD_DECLARE_BEGIN(FixedDict)
SCRIPT_METHOD_DECLARE("__reduce_ex__",	reduce_ex__, METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("has_key",		has_key, METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("keys",			keys, METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("values",			values, METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE("items",			items, METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE_END()


SCRIPT_MEMBER_DECLARE_BEGIN(FixedDict)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(FixedDict)
SCRIPT_GETSET_DECLARE_END()
SCRIPT_INIT(FixedDict, 0, 0, &FixedDict::mappingMethods, 0, 0)	
	
//-------------------------------------------------------------------------------------
FixedDict::FixedDict(DataType* dataType, std::string& strDictInitData):
Map(getScriptType(), false)
{
	_dataType = static_cast<FixedDictType*>(dataType);
	_dataType->incRef();
	initialize(strDictInitData);
}

//-------------------------------------------------------------------------------------
FixedDict::FixedDict(DataType* dataType, PyObject* pyDictInitData):
Map(getScriptType(), false)
{
	_dataType = static_cast<FixedDictType*>(dataType);
	_dataType->incRef();
	initialize(pyDictInitData);
}

//-------------------------------------------------------------------------------------
FixedDict::FixedDict(DataType* dataType):
Map(getScriptType(), false)
{
	_dataType = static_cast<FixedDictType*>(dataType);
	_dataType->incRef();
	initialize("");
}


//-------------------------------------------------------------------------------------
FixedDict::~FixedDict()
{
	_dataType->decRef();
}

//-------------------------------------------------------------------------------------
void FixedDict::initialize(std::string strDictInitData)
{
	FixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = _dataType->getKeyTypes();
	FixedDictType::FIXEDDICT_KEYTYPE_MAP::iterator iter = keyTypes.begin();
	for(; iter != keyTypes.end(); iter++)
	{
		PyObject* pyobj = iter->second->createObject(NULL);
		if(pyobj)
		{
			PyDict_SetItem(pyDict_, PyUnicode_FromString(iter->first.c_str()), pyobj);
		}
		else
		{
			ERROR_MSG("FixedDict::initialize: is error! strDictInitData=%s.\n", strDictInitData.c_str());
		}
	}
}

//-------------------------------------------------------------------------------------
void FixedDict::initialize(PyObject* pyDictInitData)
{
	if(pyDictInitData)
		PyDict_Update(pyDict_, pyDictInitData);
}

//-------------------------------------------------------------------------------------
PyObject* FixedDict::__py_reduce_ex__(PyObject* self, PyObject* protocol)
{
	FixedDict* fixedDict = static_cast<FixedDict*>(self);
	PyObject* args = PyTuple_New(2);
	PyObject* unpickleMethod = script::Pickler::getUnpickleFunc("FixedDict");
	PyTuple_SET_ITEM(args, 0, unpickleMethod);
	PyObject* args1 = PyTuple_New(1);
	PyTuple_SET_ITEM(args1, 0, PyDict_Copy(fixedDict->getDictObject()));
	PyTuple_SET_ITEM(args, 1, args1);

	if(unpickleMethod == NULL){
		Py_DECREF(args);
		return NULL;
	}
	return args;
}

//-------------------------------------------------------------------------------------
PyObject* FixedDict::__unpickle__(PyObject* self, PyObject* args)
{
	Py_ssize_t size = PyTuple_Size(args);
	if(size != 1)
	{
		ERROR_MSG("FixedDict::__unpickle__: args is error! size != 1");
		S_Return;
	}

	PyObject* dict = PyTuple_GET_ITEM(args, 0);
	if(dict == NULL)
	{
		ERROR_MSG("FixedDict::__unpickle__: args is error!");
		S_Return;
	}
	
	Py_INCREF(dict);
	return dict;
}

//-------------------------------------------------------------------------------------
void FixedDict::onInstallScript(PyObject* mod)
{
	static PyMethodDef __unpickle__Method = {"FixedDict", (PyCFunction)&FixedDict::__unpickle__, METH_VARARGS, 0};
	PyObject* pyFunc = PyCFunction_New(&__unpickle__Method, NULL);
	script::Pickler::registerUnpickleFunc(pyFunc, "FixedDict");
	Py_DECREF(pyFunc);
}


//-------------------------------------------------------------------------------------
int FixedDict::mp_length(PyObject* self)
{
	return PyDict_Size(static_cast<FixedDict*>(self)->pyDict_);
}

//-------------------------------------------------------------------------------------
int FixedDict::mp_ass_subscript(PyObject* self, PyObject* key, PyObject* value)
{
	const char* dictKeyName = PyBytes_AsString(key);
	FixedDict* fixedDict = static_cast<FixedDict*>(self);
	if (value == NULL)
	{
		if(!fixedDict->checkDataChanged(dictKeyName, value, true))
			return 0;
		return PyDict_DelItem(fixedDict->pyDict_, key);
	}
	
	if(!fixedDict->checkDataChanged(dictKeyName, value))
		return 0;
	return PyDict_SetItem(fixedDict->pyDict_, key, value);
}

//-------------------------------------------------------------------------------------
bool FixedDict::checkDataChanged(const char* keyName, PyObject* value, bool isDelete)
{
	FixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = _dataType->getKeyTypes();
	FixedDictType::FIXEDDICT_KEYTYPE_MAP::const_iterator iter = keyTypes.begin();
	
	for(; iter != keyTypes.end(); iter++)
	{
		if((*iter).first == keyName)
		{
			if(isDelete)
			{
				char err[255];
				kbe_snprintf(err, 255, "can't delete from FIXED_DICT key[%s].\n", keyName);
				PyErr_SetString(PyExc_TypeError, err);
				PyErr_PrintEx(0);
				return false;
			}
			else
			{
				DataType* dataType = (*iter).second;
				if(!dataType->isSameType(value)){
					return false;
				}
			}

			return true;
		}
	}

	char err[255];
	kbe_snprintf(err, 255, "set FIXED_DICT to a unknown key[%s].\n", keyName);
	PyErr_SetString(PyExc_TypeError, err);
	PyErr_PrintEx(0);
	return false;
}
	
//-------------------------------------------------------------------------------------
PyObject* FixedDict::mp_subscript(PyObject* self, PyObject* key)
{
	FixedDict* fixedDict = static_cast<FixedDict*>(self);

	PyObject* pyObj = PyDict_GetItem(fixedDict->pyDict_, key);
	if (!pyObj)
		PyErr_SetObject(PyExc_KeyError, key);
	else
		Py_INCREF(pyObj);
	return pyObj;
}

//-------------------------------------------------------------------------------------
PyObject* FixedDict::update(PyObject* args)
{
	return script::Map::__py_update(this, args);
}
	
//-------------------------------------------------------------------------------------
}
