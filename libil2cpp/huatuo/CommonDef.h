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
}


