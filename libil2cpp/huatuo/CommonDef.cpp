#include <iostream>

#include "CommonDef.h"


namespace huatuo
{
    void LogPanic(const char* errMsg)
    {
        std::cerr << "panic:" << std::endl;
        std::cerr << "\t" << errMsg << std::endl;
        exit(1);
    }

    const char* GetAssemblyNameFromPath(const char* assPath)
    {
        const char* last = nullptr;
        for (const char* p = assPath; *p; p++)
        {
            if (*p == '/' || *p == '\\')
            {
                last = p + 1;
            }
        }
        return last ? last : assPath;
    }
}




