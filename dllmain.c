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
	//Check! Debug mode!
	int debug_mode = 0;
	if (buf1[0] == '!'){
		buf1++;
		debug_mode = 1;
	}
	//�������
	int len = strlen(buf1) + 1;
	char * strCopy = NULL;
	strCopy = (char *)malloc((len)* sizeof (char));
	if (strCopy == NULL){
		if (debug_mode){
			strcpy_s(buf2, size, "Can't memory malloc");
		}
		return -1;
	}
	strcpy_s(strCopy, size, buf1);
	//����
	char * module_name = strCopy;
	char * func_name = NULL;
	char * data = NULL;
	for (int i = 0; strCopy[i] != '\0'; i++) {
		if (strCopy[i] == '|' && func_name == NULL){
			strCopy[i] = '\0';
			i++;
			func_name = &strCopy[i];
		}
		else if (strCopy[i] == '|') {
			strCopy[i] = '\0';
			data = &strCopy[i + 1];
			break;
		}
	}
	//������� ��

	if (func_name == NULL){
		free(strCopy);
		if (debug_mode){
			strcpy_s(buf2, size, "No function name");
		}
		return -2;
	}
	if (func_name[0] == '\0'){
		free(strCopy);
		if (debug_mode){
			strcpy_s(buf2, size, "Empty function name");
		}
		return -3;
	}

	if (module_name == NULL){
		free(strCopy);
		if (debug_mode){
			strcpy_s(buf2, size, "No module name");
		}
		return -4;
	}
	if (module_name[0] == '\0'){
		free(strCopy);
		if (debug_mode){
			strcpy_s(buf2, size, "Empty module name");
		}
		return -5;
	}
	char * result = NULL;
	result = python_func_get_str(module_name, func_name, data);
	if (result == NULL){
		free(strCopy);
		if (debug_mode){
			strcpy_s(buf2, size, "Python result NULL");
		}
		return -6;
	}
	if (strcmp(result, "None") == 0)
	{	
		free(strCopy);
		if (debug_mode){
			strcpy_s(buf2, size, "Python return None");
		}
		return -7;
	}
	strcpy_s(buf2, size, result);
	free(strCopy);
	return 0;
}

PyObject* python_init(char* module_name){
	// ���������������� ������������� Python
	Py_Initialize();
	PyObject *pDict, *pName, *pModule;

	do {

		pName = PyUnicode_FromString(module_name);
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
		return pDict;
	} while (0);
	Py_XDECREF(pName);
		PyErr_Print();
	return NULL;
}

char* python_func_get_str(char* module_name, char* func_name, char *val) {
	char *ret = NULL;
	PyObject *pObjct, *pDict, *pVal;
	pDict = python_init(module_name);
	// �������� ������� �������
	pObjct = PyDict_GetItemString(pDict, (const char *) func_name);
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