#include "InterpreterModule.h"


#include "Interpreter.h"

#include <unordered_map>

#include "vm/GlobalMetadata.h"
#include "vm/MetadataLock.h"
#include "vm/Class.h"

#include "MethodBridge.h"
#include "../metadata/MetadataModule.h"
#include "../metadata/MetadataUtil.h"
#include "../transform/transform.h"




namespace huatuo
{
namespace interpreter
{
	il2cpp::os::ThreadLocalValue InterpreterModule::s_machineState;

	struct CStringHash {
		size_t operator()(const char* _Keyval) const noexcept {
			return std::_Hash_array_representation(_Keyval, std::strlen(_Keyval)); // map -0 to 0
		}
	};

	struct CStringEqualTo {
		bool operator()(const char* _Left, const char* _Right) const {
			return std::strcmp(_Left, _Right) == 0;
		}
	};

	static std::unordered_map<const char*, NativeCallMethod, CStringHash, CStringEqualTo> s_calls;
	static std::unordered_map<const char*, NativeInvokeMethod, CStringHash, CStringEqualTo> s_invokes;


	void InterpreterModule::Initialize()
	{
		for (size_t i = 0; ; i++)
		{
			NativeCallMethod& method = g_callStub[i];
			if (!method.signature)
			{
				break;
			}
			s_calls.insert_or_assign(method.signature, method);
		}

		for (size_t i = 0; ; i++)
		{
			NativeInvokeMethod& method = g_invokeStub[i];
			if (!method.signature)
			{
				break;
			}
			s_invokes.insert_or_assign(method.signature, method);
		}
	}

	void AppendString(char* sigBuf, size_t bufSize, size_t& pos, const char* str)
	{
		size_t len = std::strlen(str);
		if (pos + len < bufSize)
		{
			std::strcpy(sigBuf + pos, str);
			pos += len;
		}
		else
		{
			IL2CPP_ASSERT(false);
		}
	}

	void AppendSignature(const Il2CppType* type, bool returnType, char* sigBuf, size_t bufferSize, size_t& pos)
	{
		if (type->byref)
		{
			AppendString(sigBuf, bufferSize, pos, "i");
			return;
		}
		switch (type->type)
		{
		case IL2CPP_TYPE_VOID: AppendString(sigBuf, bufferSize, pos, "v"); break;
		case IL2CPP_TYPE_R4:
		case IL2CPP_TYPE_R8: AppendString(sigBuf, bufferSize, pos, "f"); break;
		case IL2CPP_TYPE_TYPEDBYREF:
		{
			if (returnType)
			{
				IL2CPP_ASSERT(sizeof(Il2CppTypedRef) == 24);
				AppendString(sigBuf, bufferSize, pos, "s3");
			}
			else
			{
				AppendString(sigBuf, bufferSize, pos, "i");
			}
			break;
		}
		case IL2CPP_TYPE_VALUETYPE:
		{
			if (returnType)
			{
				Il2CppClass* klass = il2cpp::vm::Class::FromIl2CppType(type);
				il2cpp::vm::Class::SetupFields(klass);
				if (klass->instance_size <= sizeof(Il2CppObject) + 8)
				{
					AppendString(sigBuf, bufferSize, pos, "i");
				}
				else
				{
					pos += std::sprintf(sigBuf + pos, "s%lld", (klass->instance_size - sizeof(Il2CppObject) + 7) / 8);
				}
			}
			else
			{
				AppendString(sigBuf, bufferSize, pos, "i");
			}
			break;
		}
		case IL2CPP_TYPE_GENERICINST:
		{
			if (returnType)
			{
				Il2CppClass* klass = il2cpp::vm::Class::FromIl2CppType(type);
				if (klass->valuetype && klass->instance_size > sizeof(Il2CppObject) + 8)
				{
					pos += std::sprintf(sigBuf + pos, "s%lld", (klass->instance_size - sizeof(Il2CppObject) + 7) / 8);
				}
				else
				{
					AppendString(sigBuf, bufferSize, pos, "i");
				}
			}
			else
			{
				AppendString(sigBuf, bufferSize, pos, "i");
			}
			break;
		}
		default: AppendString(sigBuf, bufferSize, pos, "i"); break;
		}
	}

	bool InterpreterModule::ComputSignature(const Il2CppType* ret, const Il2CppType* params, uint32_t paramCount, bool instanceCall, char* sigBuf, size_t bufferSize)
	{
		size_t pos = 0;
		AppendSignature(ret, true, sigBuf, bufferSize, pos);

		if (instanceCall)
		{
			AppendString(sigBuf, bufferSize, pos, "i");
		}

		for (uint8_t i = 0; i < paramCount; i++)
		{
			AppendSignature(params + i, false, sigBuf, bufferSize, pos);
		}
		sigBuf[pos] = 0;
		return true;
	}

	bool InterpreterModule::ComputSignature(const Il2CppMethodDefinition* method, bool call, char* sigBuf, size_t bufferSize)
	{
		size_t pos = 0;

		const Il2CppImage* image = huatuo::metadata::MetadataModule::GetImage(method)->GetIl2CppImage();

		AppendSignature(huatuo::metadata::MetadataModule::GetIl2CppTypeFromEncodeIndex(method->returnType), true, sigBuf, bufferSize, pos);

		if (call && metadata::IsInstanceMethod(method))
		{
			AppendString(sigBuf, bufferSize, pos, "i");
		}

		for (uint8_t i = 0; i < method->parameterCount; i++)
		{
			TypeIndex paramTypeIndex = huatuo::metadata::MetadataModule::GetParameterDefinitionFromIndex(image, method->parameterStart + i)->typeIndex;
			AppendSignature(huatuo::metadata::MetadataModule::GetIl2CppTypeFromEncodeIndex(paramTypeIndex), false, sigBuf, bufferSize, pos);
		}
		sigBuf[pos] = 0;
		return true;
	}

	bool InterpreterModule::ComputSignature(const MethodInfo* method, bool call, char* sigBuf, size_t bufferSize)
	{
		size_t pos = 0;

		AppendSignature(method->return_type, true, sigBuf, bufferSize, pos);

		if (call && metadata::IsInstanceMethod(method))
		{
			AppendString(sigBuf, bufferSize, pos, "i");
		}

		for (uint8_t i = 0; i < method->parameters_count; i++)
		{
			AppendSignature(method->parameters[i].parameter_type, false, sigBuf, bufferSize, pos);
		}
		sigBuf[pos] = 0;
		return true;
	}

	template<typename T>
	const NativeCallMethod* GetNativeCallMethod(const T* method, bool forceStatic)
	{
		char sigName[1000];
		InterpreterModule::ComputSignature(method, !forceStatic, sigName, sizeof(sigName) - 1);
		auto it = s_calls.find(sigName);
		return (it != s_calls.end()) ? &it->second : nullptr;
	}

	template<typename T>
	const NativeInvokeMethod* GetNativeInvokeMethod(const T* method)
	{
		char sigName[1000];
		InterpreterModule::ComputSignature(method, false, sigName, sizeof(sigName) - 1);
		auto it = s_invokes.find(sigName);
		return (it != s_invokes.end()) ? &it->second : nullptr;
	}

	static void NotSupportAOTSignature()
	{
		il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetTypeInitializationException("", nullptr));
	}

	Il2CppMethodPointer InterpreterModule::GetMethodPointer(const Il2CppMethodDefinition* method)
	{
		const NativeCallMethod* ncm = GetNativeCallMethod(method, false);
		if (ncm)
		{
			return ncm->method;
		}
		return NotSupportAOTSignature;
	}


	Il2CppMethodPointer InterpreterModule::GetMethodPointer(const MethodInfo* method)
	{
		const NativeCallMethod* ncm = GetNativeCallMethod(method, false);
		if (ncm)
		{
			return ncm->method;
		}
		return NotSupportAOTSignature;
	}

	Il2CppMethodPointer InterpreterModule::GetAdjustThunkMethodPointer(const Il2CppMethodDefinition* method)
	{
		if (!huatuo::metadata::IsInstanceMethod(method))
		{
			return nullptr;
		}
		const NativeCallMethod* ncm = GetNativeCallMethod(method, false);
		if (ncm)
		{
			return ncm->adjustThunkMethod;
		}
		return NotSupportAOTSignature;
	}

	Il2CppMethodPointer InterpreterModule::GetAdjustThunkMethodPointer(const MethodInfo* method)
	{
		if (!huatuo::metadata::IsInstanceMethod(method))
		{
			return nullptr;
		}
		const NativeCallMethod* ncm = GetNativeCallMethod(method, false);
		if (ncm)
		{
			return ncm->adjustThunkMethod;
		}
		return NotSupportAOTSignature;
	}

	void NotSupportManaged2Native(const MethodInfo* method, uint16_t* argVarIndexs, StackObject* localVarBase, void* ret)
	{
		il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetTypeInitializationException("", nullptr));
	}

	Managed2NativeCallMethod InterpreterModule::GetManaged2NativeMethodPointer(const MethodInfo* method, bool forceStatic)
	{
		const NativeCallMethod* ncm = GetNativeCallMethod(method, forceStatic);
		if (ncm)
		{
			return ncm->managed2NativeMethod;
		}
		return NotSupportManaged2Native;
	}

	Managed2NativeCallMethod InterpreterModule::GetManaged2NativeMethodPointer(const metadata::ResolveStandAloneMethodSig& method)
	{
		char sigName[1000];
		ComputSignature(&method.returnType, method.params, method.paramCount, false, sigName, sizeof(sigName) - 1);
		auto it = s_calls.find(sigName);
		if (it != s_calls.end())
		{
			return it->second.managed2NativeMethod;
		}
		return NotSupportManaged2Native;
	}

	static void* NotSupportInvoke(Il2CppMethodPointer, const MethodInfo*, void*, void**)
	{
		il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetTypeInitializationException("", nullptr));
		return nullptr;
	}

	InvokerMethod InterpreterModule::GetMethodInvoker(const Il2CppMethodDefinition* method)
	{
		const NativeInvokeMethod* nim = GetNativeInvokeMethod(method);
		if (nim)
		{
			return huatuo::metadata::IsInstanceMethod(method) ? nim->instanceMethod : nim->staticMethod;
		}
		return NotSupportInvoke;
	}

	InvokerMethod InterpreterModule::GetMethodInvoker(const MethodInfo* method)
	{
		const NativeInvokeMethod* nim = GetNativeInvokeMethod(method);
		if (nim)
		{
			return huatuo::metadata::IsInstanceMethod(method) ? nim->instanceMethod : nim->staticMethod;
		}
		return NotSupportInvoke;
	}

	InterpMethodInfo* InterpreterModule::GetInterpMethodInfo(metadata::Image* image, const MethodInfo* methodInfo)
	{
		il2cpp::os::FastAutoLock lock(&il2cpp::vm::g_MetadataLock);

		if (methodInfo->huatuoData)
		{
			return (InterpMethodInfo*)methodInfo->huatuoData;
		}

		metadata::MethodBody& originMethod = image->GetMethodBody(methodInfo->token);
		InterpMethodInfo* imi = new (IL2CPP_MALLOC_ZERO(sizeof(InterpMethodInfo))) InterpMethodInfo;
		transform::HiTransform::Transform(image, methodInfo, originMethod, *imi);
		il2cpp::os::Atomic::FullMemoryBarrier();
		const_cast<MethodInfo*>(methodInfo)->huatuoData = imi;
		return imi;
	}


}
}

