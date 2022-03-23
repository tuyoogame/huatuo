#include "il2cpp-config.h"

#if !RUNTIME_TINY

#include "NativeDelegateMethodCache.h"
#include "os/Mutex.h"

namespace il2cpp
{
namespace utils
{
    baselib::ReentrantLock NativeDelegateMethodCache::m_CacheMutex;
    NativeDelegateMap NativeDelegateMethodCache::m_NativeDelegateMethods;

    const VmMethod* NativeDelegateMethodCache::GetNativeDelegate(Il2CppMethodPointer nativeFunctionPointer)
    {
        os::FastAutoLock lock(&m_CacheMutex);

        NativeDelegateMap::iterator i = m_NativeDelegateMethods.find(nativeFunctionPointer);
        if (i == m_NativeDelegateMethods.end())
            return NULL;

        return i->second;
    }

    void NativeDelegateMethodCache::AddNativeDelegate(Il2CppMethodPointer nativeFunctionPointer, const VmMethod* managedMethodInfo)
    {
        os::FastAutoLock lock(&m_CacheMutex);
        m_NativeDelegateMethods.insert(std::make_pair(nativeFunctionPointer, managedMethodInfo));
    }
} // namespace utils
} // namespace il2cpp

#endif
