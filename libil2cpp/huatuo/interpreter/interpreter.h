#pragma once
#include <vector>

#include "os/ThreadLocalValue.h"

#include "Engine.h"
#include "MethodBridge.h"
#include "../metadata/MetadataDef.h"
#include "../metadata/Image.h"

namespace huatuo
{

namespace interpreter
{

	struct InterpExceptionClause
	{
		metadata::CorILExceptionClauseType flags;
		int32_t tryBeginOffset;
		int32_t tryEndOffset;
		int32_t handlerBeginOffset;
		int32_t handlerEndOffset;
		int32_t filterBeginOffset;
		Il2CppClass* exKlass;
	};

	// from obj or arg
	enum class LocationDataType
	{
		I1,
		U1,
		I2,
		U2,
		U8,
		S_16,
		S_24,
		S_32,
		S_N,
	};

	struct ArgDesc
	{
		LocationDataType type;
		uint32_t stackObjectSize; //
	};

	struct InterpMethodInfo
	{
		const MethodInfo* method;
		ArgDesc* args;
		uint32_t argCount;
		uint32_t argStackObjectSize;
		byte* codes;
		uint32_t codeLength;
		uint32_t maxStackSize; // args + locals + evalstack size
		uint32_t localVarBaseOffset;
		uint32_t evalStackBaseOffset;
		uint32_t localStackSize; // args + locals StackObject size
		std::vector<const void*> resolveDatas;
		std::vector<InterpExceptionClause*> exClauses;
		uint32_t isTrivialCopyArgs : 1;
	};

	class Interpreter
	{
	public:
		static void RuntimeClassCCtorInit(const MethodInfo* method)
		{
			Il2CppClass* klass = method->klass;
			if (klass->has_cctor && !klass->cctor_finished && !huatuo::metadata::IsInstanceMethod(method))
			{
				il2cpp_codegen_runtime_class_init(klass);
			}
		}

		static void RuntimeClassCCtorInit(Il2CppClass* klass)
		{
			if (klass->has_cctor && !klass->cctor_finished)
			{
				il2cpp_codegen_runtime_class_init(klass);
			}
		}

		static void Execute(const MethodInfo* methodInfo, StackObject* args, StackObject* ret);

	};

}
}

