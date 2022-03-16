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
	int len = strlen(buf1) + 1;
	char * strCopy = NULL;
	strCopy = (char *)malloc((len)* sizeof (char));
	if (strCopy == NULL){
		strcpy_s(buf2, size, "no copy");
		return -1;
	}
	strcpy_s(strCopy, size, buf1);
	//Режь
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
	//Проверь всё
	if (data == NULL){
		free(strCopy);
		strcpy_s(buf2, size, "no function");
		return -2;
	}
	if (data[0] == '\0'){
		free(strCopy);
		strcpy_s(buf2, size, "empty data");
		return -3;
	}
	if (func_name == NULL){
		free(strCopy);
		strcpy_s(buf2, size, "no module");
		return -4;
	}
	if (func_name[0] == '\0'){
		free(strCopy);
		strcpy_s(buf2, size, "empty func");
		return -5;
	}
	if (func_name == NULL){
		free(strCopy);
		strcpy_s(buf2, size, "no module");
		return -6;
	}
	if (func_name[0] == '\0'){
		free(strCopy);
		strcpy_s(buf2, size, "empty func");
		return -7;
	}
	if (module_name[0] == '\0'){
		free(strCopy);
		strcpy_s(buf2, size, "empty module");
		return -8;
	}
	char * result = NULL;
	result = python_func_get_str(module_name, func_name, data);
	if (result == NULL){
		free(strCopy);
		return -4;
	}
	if (strcmp(result, "None") == 0)
	{	
		free(strCopy);
		return -5;
	}
	strcpy_s(buf2, size, result);
	free(strCopy);
	return 0;
}

PyObject* python_init(char* module_name){
	// Инициализировать интерпретатор Python
	Py_Initialize();
	PyObject *pDict, *pName, *pModule;

	do {

		pName = PyUnicode_FromString(module_name);
		if (!pName) {
			break;
		}

		// Загрузить объект модуля
		pModule = PyImport_Import(pName);
		if (!pModule) {
			break;
		}

		// Словарь объектов содержащихся в модуле
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
	// Загрузка объекта get_value из func.py
	pObjct = PyDict_GetItemString(pDict,  func_name);
	if (!pObjct) {
		return ret;
	}

	do {
		// Проверка pObjct на годность.
		if (!PyCallable_Check(pObjct)) {
			break;
		}

		pVal = PyObject_CallFunction(pObjct, (char *) "(s)", val);
		if (pVal != NULL) {
			PyObject* pResultRepr = PyObject_Str(pVal);

			// Если полученную строку не скопировать, то после очистки ресурсов Python её не будет.
			// Для начала pResultRepr нужно привести к массиву байтов.
			ret = _strdup(PyBytes_AS_STRING(PyUnicode_AsEncodedString(pResultRepr, "utf-8", "ERROR")));

			Py_XDECREF(pResultRepr);
			Py_XDECREF(pVal);
		}
		else {
			PyErr_Print();
		}
	} while (0);
	// Вернуть ресурсы системе
	Py_XDECREF(pObjct);
	Py_XDECREF(pDict);


	// Выгрузка интерпретатора Python
	Py_Finalize();
	return ret;
}