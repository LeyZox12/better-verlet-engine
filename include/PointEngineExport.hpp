#pragma once

#ifdef POINTENGINE_EXPORTS
    #define POINTENGINE_API __declspec(dllexport)
#else
    #define POINTENGINE_API __declspec(dllimport)
#endif