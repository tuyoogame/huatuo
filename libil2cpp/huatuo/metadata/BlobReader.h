#pragma once

#include "../CommonDef.h"

namespace huatuo
{
namespace metadata
{
    class Image;
    class BlobReader
    {
    public:
        BlobReader(Image& image, const byte* buf, uint32_t length) : _image(image), _buf(buf), _length(length), _readPosition(0)
        {

        }

        Image& GetImage() const
        {
            return _image;
        }

        const byte* GetData() const
        {
            return _buf;
        }

        uint32_t GetLength() const
        {
            return _length;
        }

        uint32_t GetReadPosition() const
        {
            return _readPosition;
        }

        bool IsEmpty() const
        {
            return _readPosition >= _length;
        }

        bool NonEmpty() const
        {
            return _readPosition < _length;
        }

        int32_t ReadCompressedInt32()
        {
            uint32_t uval = ReadCompressedUint32();
            uint32_t ival = uval >> 1;
            if (!(uval & 1))
                return ival;
            if (ival < 0x40)
                return ival - 0x40;
            if (ival < 0x2000)
                return ival - 0x2000;
            if (ival < 0x10000000)
                return ival - 0x10000000;
            IL2CPP_ASSERT(ival < 0x20000000);
            //g_warning("compressed signed value appears to use 29 bits for compressed representation: %x (raw: %8x)", ival, uval);
            return ival - 0x20000000;
        }

        uint32_t ReadCompressedUint32()
        {
            const unsigned char* ptr = (const unsigned char*)(_buf + _readPosition);
            uint32_t b = *ptr;
            uint32_t len;

            if ((b & 0x80) == 0) {
                len = b;
                ++_readPosition;
            }
            else if ((b & 0x40) == 0) {
                len = (((uint32_t)b & 0x3f) << 8 | (uint32_t)ptr[1]);
                _readPosition += 2;
            }
            else {
                len = (((uint32_t)b & 0x1f) << 24) |
                    ((uint32_t)ptr[1] << 16) |
                    ((uint32_t)ptr[2] << 8) |
                        (uint32_t)ptr[3];
                _readPosition += 4;
            }
            return len;
        }

        uint8_t ReadByte()
        {
            IL2CPP_ASSERT(_readPosition < _length);
            return _buf[_readPosition++];
        }

        uint16_t ReadUshort()
        {
            IL2CPP_ASSERT(_readPosition + 2 <= _length);
            uint16_t value = *(uint16_t*)(_buf + _readPosition);
            _readPosition += 2;
            return value;
        }

        template<typename T>
        T Read()
        {
            IL2CPP_ASSERT(_readPosition + sizeof(T) <= _length);
            T value = *(T*)(_buf + _readPosition);
            _readPosition += sizeof(T);
            return value;
        }

        uint8_t PeekByte()
        {
            IL2CPP_ASSERT(_readPosition < _length);
            return _buf[_readPosition];
        }

        void SkipByte()
        {
            IL2CPP_ASSERT(_readPosition < _length);
            ++_readPosition;
        }

        const byte* GetAndSkipCurBytes(uint32_t len)
        {
            IL2CPP_ASSERT(_readPosition + len <= _length);
            const byte* data = _buf + _readPosition;
            _readPosition += len;
            return data;
        }

    private:
        Image& _image;
        const byte* const _buf;
        const uint32_t _length;
        uint32_t _readPosition;
    };

}
}