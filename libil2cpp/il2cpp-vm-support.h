#pragma once

#include "il2cpp-config.h"

// This file is a compile-time abstraction for VM support needed by code in the os and utils namespaces that is used by both the
// libil2cpp and the libil2cpptiny runtimes. Code in those namespaces should never depend up on the vm namespace directly.

#if RUNTIME_NONE // OS layer compiled with no runtime
    #define IL2CPP_VM_RAISE_EXCEPTION(exception) IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_RAISE_COM_EXCEPTION(hresult, defaultToCOMException) IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_RAISE_UNAUTHORIZED_ACCESS_EXCEPTION(message) IL2CPP_ASSERT(0 && message)
    #define IL2CPP_VM_RAISE_PLATFORM_NOT_SUPPORTED_EXCEPTION(message) IL2CPP_ASSERT(0 && message)
    #define IL2CPP_VM_RAISE_IF_FAILED(hresult, defaultToCOMException) IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_STRING_EMPTY() NULL
    #define IL2CPP_VM_STRING_NEW_UTF16(value, length) NULL
    #define IL2CPP_VM_STRING_NEW_LEN(value, length) NULL
    #define IL2CPP_VM_NOT_SUPPORTED(func, reason) IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_NOT_IMPLEMENTED(func) IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_METHOD_METADATA_FROM_METHOD_KEY(key) IL2CPP_ASSERT(0 && "This is not implemented wihout a VM runtime backend.")
    #define IL2CPP_VM_SHUTDOWN() IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_GET_CREATE_CCW_EXCEPTION(ex) NULL
    #define IL2CPP_VM_PROFILE_FILEIO(kind, count) NULL
#elif RUNTIME_TINY
    #include "vm/StackTrace.h"
    #include "vm/DebugMetadata.h"
    #define IL2CPP_VM_RAISE_COM_EXCEPTION(hresult, defaultToCOMException) IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_SHUTDOWN() IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
    #define IL2CPP_VM_NOT_SUPPORTED(func, reason) IL2CPP_ASSERT(0 && "This is not implemented without a VM runtime backend.")
#if IL2CPP_TINY_DEBUG_METADATA
    #define IL2CPP_VM_METHOD_METADATA_FROM_METHOD_KEY(key) tiny::vm::DebugMetadata::GetMethodNameFromMethodDefinitionIndex(key->methodIndex)
#else
    #define IL2CPP_VM_METHOD_METADATA_FROM_METHOD_KEY(key) NULL
#endif
typedef TinyMethod VmMethod;
#else // Assume the libil2cpp runtime
    #include "vm/Exception.h"
    #include "vm/MetadataCache.h"
    #include "vm/StackTrace.h"
    #include "vm/String.h"
    #include "vm/Profiler.h"
    #define IL2CPP_VM_RAISE_EXCEPTION(exception) il2cpp::vm::Exception::Raise(exception)
    #define IL2CPP_VM_RAISE_COM_EXCEPTION(hresult, defaultToCOMException) il2cpp::vm::Exception::Raise(hresult, defaultToCOMException)
    #define IL2CPP_VM_RAISE_UNAUTHORIZED_ACCESS_EXCEPTION(message) il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetUnauthorizedAccessException(message))
    #define IL2CPP_VM_RAISE_PLATFORM_NOT_SUPPORTED_EXCEPTION(message) il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetPlatformNotSupportedException(message))
    #define IL2CPP_VM_RAISE_IF_FAILED(hresult, defaultToCOMException) il2cpp::vm::Exception::RaiseIfFailed(hresult, defaultToCOMException)
    #define IL2CPP_VM_STRING_EMPTY() il2cpp::vm::String::Empty()
    #define IL2CPP_VM_STRING_NEW_UTF16(value, length) il2cpp::vm::String::NewUtf16(value, length)
    #define IL2CPP_VM_STRING_NEW_LEN(value, length) il2cpp::vm::String::NewLen(value, length)
    #define IL2CPP_VM_NOT_SUPPORTED(func, reason) NOT_SUPPORTED_IL2CPP(func, reason)
    #define IL2CPP_VM_NOT_IMPLEMENTED(func) IL2CPP_NOT_IMPLEMENTED_ICALL(func)
    #define IL2CPP_VM_METHOD_METADATA_FROM_METHOD_KEY(key) il2cpp::vm::MetadataCache::GetMethodInfoFromMethodHandle (key->methodHandle)
    #define IL2CPP_VM_SHUTDOWN() il2cpp_shutdown()
    #define IL2CPP_VM_GET_CREATE_CCW_EXCEPTION(ex) vm::CCW::GetOrCreate(reinterpret_cast<Il2CppObject*>(ex), Il2CppIUnknown::IID)
    #define IL2CPP_VM_PROFILE_FILEIO(kind, count) if (il2cpp::vm::Profiler::ProfileFileIO()) il2cpp::vm::Profiler::FileIO(kind, count);

typedef Il2CppString VmString;
typedef MethodInfo VmMethod;
#endif
