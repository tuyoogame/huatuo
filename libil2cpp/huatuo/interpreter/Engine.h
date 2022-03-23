#pragma once

#include <stack>

#include "gc/GarbageCollector.h"
#include "vm/Exception.h"
#include "vm/StackTrace.h"

#include "../CommonDef.h"

#if DEBUG
#define PUSH_STACK_FRAME(method) do { \
	Il2CppStackFrameInfo stackFrameInfo = { method, (uintptr_t)method->methodPointer }; \
	il2cpp::vm::StackTrace::PushFrame(stackFrameInfo); \
} while(0)

#define POP_STACK_FRAME() do { il2cpp::vm::StackTrace::PopFrame(); } while(0)

#else 
#define PUSH_STACK_FRAME(method)
#define POP_STACK_FRAME() 
#endif

namespace huatuo
{
namespace interpreter
{

	union StackObject
	{
		void* ptr; // can't adjust position. will raise native_invoke init args bugs.
		bool b;
		int8_t i8;
		uint8_t u8;
		int16_t i16;
		uint16_t u16;
		int32_t i32;
		uint32_t u32;
		int64_t i64;
		uint64_t u64;
		float f4;
		double f8;
		Il2CppObject* obj;
		Il2CppString* str;
		Il2CppObject** ptrObj;
	};

	static_assert(sizeof(StackObject) == 8, "requrie 64bit");


	enum class ExceptionFlowType
	{
		None,
		Exception,
		Leave,
	};

	struct InterpMethodInfo;

	struct InterpFrame
	{
		const InterpMethodInfo* method;
		StackObject* stackBasePtr;
		ptrdiff_t oldStackTop;
		void* ret;

		byte* ip;

		ExceptionFlowType exFlowType;
		int32_t throwOffset;
		int32_t nextExClauseIndex;
		int32_t leaveTarget;
		//std::vector<void*> *bigLocalAllocs;
	};

	const uint32_t kMaxStackObjectCount = 1024 * 128;
	const uint32_t kMaxFrameCount = 1024;

	class MachineState
	{
	public:
		MachineState()
		{
			_stackSize = kMaxStackObjectCount;
			_stackBase = (StackObject*)il2cpp::gc::GarbageCollector::AllocateFixed(kMaxStackObjectCount * sizeof(StackObject), nullptr);
			std::memset(_stackBase, 0, _stackSize * sizeof(StackObject));
			_stackTopIdx = 0;

			_frameBase = (InterpFrame*)IL2CPP_CALLOC(kMaxFrameCount, sizeof(InterpFrame));
			_frameCount = kMaxFrameCount;
			_frameTopIdx = 0;
		}

		~MachineState()
		{
			il2cpp::gc::GarbageCollector::FreeFixed(_stackBase);
		}

		StackObject* AllocArgments(uint32_t argCount)
		{
			if (_stackTopIdx + argCount > _stackSize)
			{
				il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetStackOverflowException("AllocArguments"));
			}
			StackObject* args = _stackBase + _stackTopIdx;
			_stackTopIdx += argCount;
			return args;
		}

		StackObject* GetStackBasePtr() const
		{
			return _stackBase;
		}

		ptrdiff_t GetStackTop() const
		{
			return _stackTopIdx;
		}

		StackObject* AllocStackSlot(uint32_t slotNum)
		{
			if (_stackTopIdx + slotNum > _stackSize)
			{
				il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetStackOverflowException("AllocStackSlot"));
			}
			StackObject* dataPtr = _stackBase + _stackTopIdx;
			_stackTopIdx += slotNum;
			return dataPtr;
		}

		void SetStackTop(ptrdiff_t oldTop)
		{
			_stackTopIdx = oldTop;
		}

		InterpFrame* PushFrame()
		{
			if (_frameTopIdx >= _frameCount)
			{
				il2cpp::vm::Exception::Raise(il2cpp::vm::Exception::GetStackOverflowException("AllocFrame"));
			}
			return _frameBase + _frameTopIdx++;
		}

		void PopFrame()
		{
			IL2CPP_ASSERT(_frameTopIdx > 0);
			--_frameTopIdx;
		}

		void PopFrameN(uint32_t count)
		{
			IL2CPP_ASSERT(count > 0 && _frameTopIdx >= count);
			_frameTopIdx -= count;
		}

	private:

		StackObject* _stackBase;
		ptrdiff_t _stackSize;
		ptrdiff_t _stackTopIdx;

		InterpFrame* _frameBase;
		uint32_t _frameTopIdx;
		uint32_t _frameCount;
	};

	class InterpFrameGroup
	{
	public:
		InterpFrameGroup(MachineState& ms) : _machineState(ms), _stackBaseIdx(ms.GetStackTop())
		{

		}

		void CleanUpFrames()
		{
			uint32_t n = (uint32_t)_frames.size();
			if (n > 0)
			{
				_machineState.SetStackTop(_stackBaseIdx);
				_machineState.PopFrameN(n);
			}
		}

		InterpFrame* EnterFrame(const InterpMethodInfo* imi, StackObject* argBase, bool withArgStack);

		InterpFrame* LeaveFrame()
		{
			IL2CPP_ASSERT(!_frames.empty());
			POP_STACK_FRAME();
			InterpFrame* frame = _frames.top();
			_machineState.PopFrame();
			_machineState.SetStackTop(frame->oldStackTop);
			_frames.pop();
			return _frames.empty() ? nullptr : _frames.top();
		}

		void* AllocLoc(size_t size)
		{
			uint32_t soNum = (uint32_t)(size + sizeof(StackObject) - 1) / sizeof(StackObject);
			return _machineState.AllocStackSlot(soNum);
 		}
	private:
		MachineState& _machineState;
		ptrdiff_t _stackBaseIdx;
		std::stack<InterpFrame*> _frames;
	};
}
}