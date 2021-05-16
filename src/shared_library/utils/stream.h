#ifndef PROJECTFARM_STREAM_H
#define PROJECTFARM_STREAM_H

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>

namespace projectfarm::shared::utils
{
    void WriteBool(std::vector<std::byte>& bytes, bool value) noexcept;
    void WriteUInt8(std::vector<std::byte>& bytes, uint8_t value) noexcept;
    void WriteUInt16(std::vector<std::byte>& bytes, uint16_t value) noexcept;
    void WriteUInt32(std::vector<std::byte>& bytes, uint32_t value) noexcept;
    void WriteInt32(std::vector<std::byte>& bytes, int32_t value) noexcept;
    void WriteUInt64(std::vector<std::byte>& bytes, uint64_t value) noexcept;
    void WriteString(std::vector<std::byte>& bytes, const std::string& value, uint32_t length) noexcept;

    bool ReadBool(const std::vector<std::byte>& bytes, uint32_t& index) noexcept;
    uint8_t ReadUInt8(const std::vector<std::byte>& bytes, uint32_t& index) noexcept;
    uint16_t ReadUInt16(const std::vector<std::byte>& bytes, uint32_t& index) noexcept;
    uint32_t ReadUInt32(const std::vector<std::byte>& bytes, uint32_t& index) noexcept;
    int32_t ReadInt32(const std::vector<std::byte>& bytes, uint32_t& index) noexcept;
    uint64_t ReadUInt64(const std::vector<std::byte>& bytes, uint32_t& index) noexcept;
    std::string ReadString(const std::vector<std::byte>& bytes, uint32_t& index) noexcept;

    bool ReadBoolFromBinaryFile(std::ifstream& fs) noexcept;
    std::string ReadStringFromBinaryFile(std::ifstream& fs) noexcept;
    uint8_t ReadUInt8FromBinaryFile(std::ifstream& fs) noexcept;
    uint32_t ReadUInt32FromBinaryFile(std::ifstream& fs) noexcept;
    int32_t ReadInt32FromBinaryFile(std::ifstream& fs) noexcept;
}

#endif
