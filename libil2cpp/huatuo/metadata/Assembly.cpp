
#include "Assembly.h"

#include <cstring>
#include <iostream>

#include "os/File.h"
#include "utils/MemoryMappedFile.h"
#include "vm/Assembly.h"
#include "vm/Image.h"
#include "vm/Class.h"
#include "vm/String.h"


#include "Image.h"
#include "MetadataModule.h"
#include "MetadataUtil.h"


#if IL2CPP_BYTE_ORDER != IL2CPP_LITTLE_ENDIAN
#error "only support litten endian"
#endif

using namespace il2cpp;

namespace huatuo
{
namespace metadata
{

    bool GetMappedFileBuffer(const char* assemblyFile, void*& buf, uint64_t& fileLength)
    {
        int err = 0;
        os::FileHandle* fh = os::File::Open(assemblyFile, FileMode::kFileModeOpen, FileAccess::kFileAccessRead, FileShare::kFileShareReadWrite, 0, &err);

        if (err != 0)
        {
            //utils::Logging::Write("ERROR: Could not open %s", assemblyFile);
            return false;
        }

        fileLength = os::File::GetLength(fh, &err);
        if (err != 0)
        {
            //utils::Logging::Write("ERROR: GetLength %s, err:%d", assemblyFile, err);
            os::File::Close(fh, &err);
            return false;
        }

        buf = utils::MemoryMappedFile::Map(fh);

        os::File::Close(fh, &err);
        if (err != 0)
        {
            //utils::Logging::Write("ERROR: Close %s, err:%ulld", assemblyFile, err);
            utils::MemoryMappedFile::Unmap(buf);
            buf = NULL;
            return false;
        }
        return true;
    }

    const char* copyString(const char* src)
    {
        size_t len = std::strlen(src);
        char* dst = (char*)IL2CPP_MALLOC(len + 1);
        std::strcpy(dst, src);
        return dst;
    }

    Il2CppAssembly* Assembly::LoadFrom(const char* assemblyFile)
    {
        void* fileBuffer;
        uint64_t fileLength;
        if (!GetMappedFileBuffer(assemblyFile, fileBuffer, fileLength))
        {
            return nullptr;
        }

        const char* assemblyName = huatuo::GetAssemblyNameFromPath(assemblyFile);
        auto ass = Create(assemblyFile, assemblyName, (const byte*)fileBuffer, fileLength);
        vm::Assembly::Register(ass);
        return ass;
    }

    Il2CppAssembly* Assembly::Create(const char* assemblyFile, const char* assemblyName, const byte* assemblyData, uint64_t length)
    {
        uint32_t imageId = MetadataModule::AllocImageIndex();
        if (imageId > kMaxLoadImageCount)
        {
            vm::Exception::Raise(vm::Exception::GetArgumentException("exceed max image index", ""));
        }
        Image* image = new Image(imageId);
        LoadImageErrorCode err = image->Load(assemblyData, (size_t)length);


        if (err != LoadImageErrorCode::OK)
        {
            char errMsg[300];
            int strLen = snprintf(errMsg, 100, "bad image. file:%s err:%d", assemblyFile, (int)err);
            vm::Exception::Raise(vm::Exception::GetArgumentException("bad image", errMsg));
        }
        auto ass = new Il2CppAssembly{};

        auto image2 = new Il2CppImage{};
        image2->name = copyString(assemblyName);
        image2->nameNoExt = copyString(assemblyName);
        image2->assembly = ass;

        image->InitBasic(image2);
        image->BuildIl2CppImage(image2);
        image->BuildIl2CppAssembly(ass);
        image->InitRuntimeMetadatas();

        ass->image = image2;
        return ass;
    }

}
}

