// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

/* Include this file instead of including <windows.h> directly. */
#ifdef NOMINMAX
    #include <windows.h>
#else
    #define NOMINMAX
    #include <windows.h>
    #undef NOMINMAX
#endif



// TODO: reference additional headers your program requires here
