// dllmain.c : Defines the entry point for the DLL application.

#include "stdafx.h"

BOOL APIENTRY DllMain (HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

__declspec(dllexport) int __cdecl StrPyStr (char *buf1, char *buf2, int size)
{
	int len = strlen(buf1);
	char * strCopy = NULL;
	strCopy = (char *)malloc((len)* sizeof (char)); // ��� �������� '\ 0' ���������� �������������� ������������ ��������
	if (strCopy == NULL){
		return -1;
	}
	strcpy_s(strCopy, size, buf1); // �������� ������ str

	char * func = strCopy;
	char * data = NULL;
	for (int i = 0; strCopy[i] != '\0'; i++) {
		if (strCopy[i] == '|') {
			strCopy[i] = '\0';
			data = &strCopy[i + 1];
			break;
		}
	}
	if (data == NULL){
		free(strCopy);
		return -2;
	}
	if (data[0] == '\0'){
		free(strCopy);
		return -3;
	}
	char * buf4 = NULL;
	buf4 = python_func_get_str(func, data);
	if (buf4 == NULL){
		free(strCopy);
		return -4;
	}
	if (strcmp(buf4, "None") == 0)
	{	
		free(strCopy);
		return -5;
	}
	strcpy_s(buf2, size, buf4);
	free(strCopy);
	return 0;
}

PyObject* python_init(char* func_name) {
	// ���������������� ������������� Python
	Py_Initialize();
	PyObject *pDict, *pName, *pModule;

	do {
		// �������� ������ sys
		//sys = PyImport_ImportModule("sys");
		//sys_path = PyObject_GetAttrString(sys, "path");
		// ���� �� ����� ���������� Python
		//folder_path = PyUnicode_FromString((const char*) "E:\\Server\\PHP7");
		//PyList_Append(sys_path, folder_path);

		// �������� func.py
		pName = PyUnicode_FromString(func_name);
		if (!pName) {
			break;
		}

		// ��������� ������ ������
		pModule = PyImport_Import(pName);
		if (!pModule) {
			break;
		}

		// ������� �������� ������������ � ������
		pDict = PyModule_GetDict(pModule);
		if (!pDict) {
			break;
		}
		Py_XDECREF(pName);
		Py_XDECREF(pModule);
		//Py_XDECREF(sys);
		//Py_XDECREF(sys_path);
		return pDict;
	} while (0);
	Py_XDECREF(pName);
	//Py_XDECREF(sys);
	//Py_XDECREF(sys_path);
	// ������ ������
	PyErr_Print();
	return NULL;
}

char* python_func_get_str(char* func_name, char *val) {
	char *ret = NULL;
	PyObject *pObjct, *pDict, *pVal;
	pDict = python_init(func_name);
	// �������� ������� get_value �� func.py
	pObjct = PyDict_GetItemString(pDict, (const char *) "main");
	if (!pObjct) {
		return ret;
	}

	do {
		// �������� pObjct �� ��������.
		if (!PyCallable_Check(pObjct)) {
			break;
		}

		pVal = PyObject_CallFunction(pObjct, (char *) "(s)", val);
		if (pVal != NULL) {
			PyObject* pResultRepr = PyObject_Str(pVal);

			// ���� ���������� ������ �� �����������, �� ����� ������� �������� Python � �� �����.
			// ��� ������ pResultRepr ����� �������� � ������� ������.
			ret = _strdup(PyBytes_AS_STRING(PyUnicode_AsEncodedString(pResultRepr, "utf-8", "ERROR")));

			Py_XDECREF(pResultRepr);
			Py_XDECREF(pVal);
		}
		else {
			PyErr_Print();
		}
	} while (0);
	// ������� ������� �������
	Py_XDECREF(pObjct);
	Py_XDECREF(pDict);


	// �������� �������������� Python
	Py_Finalize();
	return ret;
}