#pragma once

#include "../CommonDef.h"
#include "MethodBridge.h"
#include "Engine.h"
#include "../metadata/Image.h"

namespace huatuo
{
namespace interpreter
{

	class InterpreterModule
	{
	public:
		static void Initialize();


		static MachineState& GetCurrentThreadMachineState()
		{
			MachineState* state = nullptr;
			s_machineState.GetValue((void**)&state);
			if (!state)
			{
				state = new MachineState();
				s_machineState.SetValue(state);
			}
			return *state;
		}

		static InterpMethodInfo* GetInterpMethodInfo(metadata::Image* image, const MethodInfo* methodInfo);

		static bool ComputSignature(const Il2CppMethodDefinition* method, bool call, char* signatureBuffer, size_t bufferSize);
		static bool ComputSignature(const MethodInfo* method, bool call, char* sigBuf, size_t bufferSize);
		static bool ComputSignature(const Il2CppType* ret, const Il2CppType* params, uint32_t paramCount, bool instanceCall, char* sigBuf, size_t bufferSize);

		static Il2CppMethodPointer GetMethodPointer(const Il2CppMethodDefinition* method);
		static Il2CppMethodPointer GetMethodPointer(const MethodInfo* method);
		static Il2CppMethodPointer GetAdjustThunkMethodPointer(const Il2CppMethodDefinition* method);
		static Il2CppMethodPointer GetAdjustThunkMethodPointer(const MethodInfo* method);
		static Managed2NativeCallMethod GetManaged2NativeMethodPointer(const MethodInfo* method, bool forceStatic);
		static Managed2NativeCallMethod GetManaged2NativeMethodPointer(const metadata::ResolveStandAloneMethodSig& methodSig);

		static InvokerMethod GetMethodInvoker(const Il2CppMethodDefinition* method);
		static InvokerMethod GetMethodInvoker(const MethodInfo* method);

	private:
		static il2cpp::os::ThreadLocalValue s_machineState;
	};
}
}