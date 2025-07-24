#pragma once
#ifdef MK_DLL
#define MK_API __declspec(dllexport)
#else
#define MK_API __declspec(dllimport)
#endif