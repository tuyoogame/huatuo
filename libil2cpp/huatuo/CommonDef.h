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
}


