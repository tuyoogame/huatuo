#pragma once

#include "il2cpp-api-types.h"
#include "il2cpp-vm-support.h"
#include <map>

#include "Baselib.h"
#include "Cpp/ReentrantLock.h"

namespace il2cpp
{
namespace utils
{
    typedef std::map<Il2CppMethodPointer, const VmMethod*> NativeDelegateMap;

    class NativeDelegateMethodCache
    {
    public:
        static const VmMethod* GetNativeDelegate(Il2CppMethodPointer nativeFunctionPointer);
        static void AddNativeDelegate(Il2CppMethodPointer nativeFunctionPointer, const VmMethod* managedMethodInfo);
    private:
        static baselib::ReentrantLock m_CacheMutex;
        static NativeDelegateMap m_NativeDelegateMethods;
    };
} // namespace utils
} // namespace il2cpp
