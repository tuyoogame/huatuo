#pragma once

#include "../CommonDef.h"

namespace huatuo
{
namespace metadata
{

    class Assembly
    {
    public:
        static Il2CppAssembly* LoadFrom(const char* assemblyFile);

    private:
        static Il2CppAssembly* Create(const char* assemblyFile, const char* assemblyName, const byte* assemblyData, uint64_t length);
    };
}
}