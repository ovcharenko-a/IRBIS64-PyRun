// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <stdio.h>
#include <string.h>


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>



// reference additional headers your program requires here
#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyObject* python_init(char* module_name);
char* python_func_get_str(char* module_name, char* func_name, char *val);
