#pragma once

#include <stdint.h>
#include <memory>

#include "codegen/il2cpp-codegen.h"
#include "utils/Memory.h"
#include "vm/GlobalMetadataFileInternals.h"

namespace huatuo
{

    typedef uint8_t byte;

    void LogPanic(const char* errMsg);

    const char* GetAssemblyNameFromPath(const char* assPath);

    const char* copyString(const char* src);

    const char* concatNewString(const char* s1, const char* s2);

	struct CStringHash
	{
		size_t operator()(const char* s) const noexcept
		{
			uint32_t hash = 0;

			for (; *s; ++s)
			{
				hash += *s;
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}

			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);

			return hash;
		}
	};

	struct CStringEqualTo
	{
		bool operator()(const char* _Left, const char* _Right) const
		{
			return std::strcmp(_Left, _Right) == 0;
		}
	};
}


