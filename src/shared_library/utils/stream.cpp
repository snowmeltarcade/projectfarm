#include "stream.h"

namespace projectfarm::shared::utils
{
    void WriteBool(std::vector<std::byte>& bytes, bool value) noexcept
    {
        auto v = static_cast<uint8_t>(value ? 1u : 0u);
        WriteUInt8(bytes, v);
    }

    void WriteUInt8(std::vector<std::byte>& bytes, uint8_t value) noexcept
    {
        static_assert(sizeof(std::byte) == sizeof(uint8_t));

        bytes.push_back(static_cast<std::byte>(value));
    }

    void WriteUInt16(std::vector<std::byte>& bytes, uint16_t value) noexcept
    {
        bytes.push_back(static_cast<std::byte>((value & 0x0000FF00) >> 8u));
        bytes.push_back(static_cast<std::byte>((value & 0x000000FF) >> 0u));
    }

    void WriteUInt32(std::vector<std::byte>& bytes, uint32_t value) noexcept
    {
        bytes.push_back(static_cast<std::byte>((value & 0xFF000000) >> 24u));
        bytes.push_back(static_cast<std::byte>((value & 0x00FF0000) >> 16u));
        bytes.push_back(static_cast<std::byte>((value & 0x0000FF00) >> 8u));
        bytes.push_back(static_cast<std::byte>((value & 0x000000FF) >> 0u));
    }

    void WriteInt32(std::vector<std::byte>& bytes, int32_t value) noexcept
    {
        bytes.push_back(static_cast<std::byte>((value & 0xFF000000) >> 24u));
        bytes.push_back(static_cast<std::byte>((value & 0x00FF0000) >> 16u));
        bytes.push_back(static_cast<std::byte>((value & 0x0000FF00) >> 8u));
        bytes.push_back(static_cast<std::byte>((value & 0x000000FF) >> 0u));
    }

    void WriteUInt64(std::vector<std::byte>& bytes, uint64_t value) noexcept
    {
        bytes.push_back(static_cast<std::byte>((value & 0xFF00000000000000) >> 56u));
        bytes.push_back(static_cast<std::byte>((value & 0x00FF000000000000) >> 48u));
        bytes.push_back(static_cast<std::byte>((value & 0x0000FF0000000000) >> 40u));
        bytes.push_back(static_cast<std::byte>((value & 0x000000FF00000000) >> 32u));
        bytes.push_back(static_cast<std::byte>((value & 0x00000000FF000000) >> 24u));
        bytes.push_back(static_cast<std::byte>((value & 0x0000000000FF0000) >> 16u));
        bytes.push_back(static_cast<std::byte>((value & 0x000000000000FF00) >> 8u));
        bytes.push_back(static_cast<std::byte>((value & 0x00000000000000FF) >> 0u));
    }

    void WriteString(std::vector<std::byte>& bytes, const std::string& value, uint32_t length) noexcept
    {
        WriteUInt32(bytes, length);

        for (auto i = 0u; i < length; ++i)
        {
            if (i >= value.size())
            {
                WriteUInt8(bytes, '\0');
            }
            else
            {
                WriteUInt8(bytes, static_cast<uint8_t>(value[i]));
            }
        }
    }

    bool ReadBool(const std::vector<std::byte>& bytes, uint32_t& index) noexcept
    {
        return ReadUInt8(bytes, index) != 0;
    }

    uint8_t ReadUInt8(const std::vector<std::byte>& bytes, uint32_t& index) noexcept
    {
        static_assert(sizeof(std::byte) == sizeof(uint8_t));

        auto value = static_cast<uint8_t>(bytes[index]);

        index += sizeof(uint8_t);

        return value;
    }

    uint16_t ReadUInt16(const std::vector<std::byte>& bytes, uint32_t& index) noexcept
    {
        auto i = static_cast<unsigned int>(index);

        auto value = (static_cast<uint64_t>(bytes[i + 2]) << 8ul) |
                     (static_cast<uint64_t>(bytes[i + 3]) << 0ul);

        index += sizeof(uint16_t);

        return static_cast<uint16_t>(value);
    }

    uint32_t ReadUInt32(const std::vector<std::byte>& bytes, uint32_t& index) noexcept
    {
        auto i = static_cast<unsigned int>(index);

        auto value = (static_cast<uint64_t>(bytes[i + 0]) << 24ul) |
                     (static_cast<uint64_t>(bytes[i + 1]) << 16ul) |
                     (static_cast<uint64_t>(bytes[i + 2]) << 8ul) |
                     (static_cast<uint64_t>(bytes[i + 3]) << 0ul);

        index += sizeof(uint32_t);

        return static_cast<uint32_t>(value);
    }

    int32_t ReadInt32(const std::vector<std::byte>& bytes, uint32_t& index) noexcept
    {
        auto i = static_cast<unsigned int>(index);

        auto value = (static_cast<int64_t>(bytes[i + 0]) << 24ul) |
                     (static_cast<int64_t>(bytes[i + 1]) << 16ul) |
                     (static_cast<int64_t>(bytes[i + 2]) << 8ul) |
                     (static_cast<int64_t>(bytes[i + 3]) << 0ul);

        index += sizeof(uint32_t);

        return static_cast<int32_t>(value);
    }

    uint64_t ReadUInt64(const std::vector<std::byte>& bytes, uint32_t& index) noexcept
    {
        auto i = static_cast<unsigned int>(index);

        const uint64_t value = (static_cast<uint64_t>(bytes[i + 0]) << 56ul) |
                               (static_cast<uint64_t>(bytes[i + 1]) << 48ul) |
                               (static_cast<uint64_t>(bytes[i + 2]) << 40ul) |
                               (static_cast<uint64_t>(bytes[i + 3]) << 32ul) |
                               (static_cast<uint64_t>(bytes[i + 4]) << 24ul) |
                               (static_cast<uint64_t>(bytes[i + 5]) << 16ul) |
                               (static_cast<uint64_t>(bytes[i + 6]) << 8ul) |
                               (static_cast<uint64_t>(bytes[i + 7]) << 0ul);

        index += sizeof(uint64_t);

        return value;
    }

    std::string ReadString(const std::vector<std::byte>& bytes, uint32_t& index) noexcept
    {
        std::string s;

        uint32_t length = ReadUInt32(bytes, index);

        for (auto i = 0u; i < length; ++i)
        {
            auto c = ReadUInt8(bytes, index);

            if (c == '\0')
            {
                index += length - i;
                break;
            }

            s += c;
        }

        return s;
    }

    bool ReadBoolFromBinaryFile(std::ifstream& fs) noexcept
    {
        return ReadUInt8FromBinaryFile(fs) != 0;
    }

    std::string ReadStringFromBinaryFile(std::ifstream& fs) noexcept
    {
        std::string value;

        char byte {0};

        fs.read(&byte, sizeof(byte));

        auto counter = byte;
        for (auto i = 0; i < counter; ++i)
        {
            fs.read(&byte, sizeof(byte));

            value += byte;
        }

        return value;
    }

    uint8_t ReadUInt8FromBinaryFile(std::ifstream& fs) noexcept
    {
        uint8_t value {0};

        fs.read(reinterpret_cast<char*>(&value), sizeof(value));

        return value;
    }

    uint32_t ReadUInt32FromBinaryFile(std::ifstream& fs) noexcept
    {
        uint32_t value {0};

        fs.read(reinterpret_cast<char*>(&value), sizeof(value));

        return value;
    }

    int32_t ReadInt32FromBinaryFile(std::ifstream& fs) noexcept
    {
        int32_t value {0};

        fs.read(reinterpret_cast<char*>(&value), sizeof(value));

        return value;
    }
}