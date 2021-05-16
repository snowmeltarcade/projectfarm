#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <memory>
#include <sstream>
#include <string>

#include "utils/util.h"
#include "packet_types.h"

namespace projectfarm::shared::networking
{
	class Packet
	{
	public:
		Packet() = default;
		virtual ~Packet() = default;

		[[nodiscard]] virtual PacketTypes GetPacketType() const = 0;

        [[nodiscard]] uint32_t PacketSize() const noexcept
        {
            return sizeof(uint32_t) + // size of the packet
                   sizeof(uint8_t) + // packet type
                   this->SizeInBytes();
        }

		[[nodiscard]] std::vector<std::byte> GetBytes() const noexcept
        {
            std::vector<std::byte> bytes;
            bytes.reserve(this->PacketSize());

            pfu::WriteUInt32(bytes, this->PacketSize());
            pfu::WriteUInt8(bytes, static_cast<uint8_t>(this->GetPacketType()));

            this->SerializeBytes(bytes);

            return bytes;
        }

		virtual void FromBytes(const std::vector<std::byte>& bytes) = 0;

		[[nodiscard]] std::string GetDebugData() const
        {
            std::stringstream ss;

            Packet::SerializeDebugData(ss, "Packet Size", this->SizeInBytes());
            Packet::SerializeDebugData(ss, "Packet Type", static_cast<int>(this->GetPacketType()));

            this->OutputDebugData(ss);

            return ss.str();
        }

		// if this returns true, this packet will send over TCP, else
		// will be sent over UDP
		[[nodiscard]] virtual bool IsVital() const = 0;

    protected:
        [[nodiscard]] virtual uint32_t SizeInBytes() const = 0;

        virtual void OutputDebugData(std::stringstream& ss) const noexcept = 0;

        template <typename T>
        static void SerializeDebugData(std::stringstream& ss, std::string_view label, T value) noexcept
        {
            ss << label << ": " << value << "\n";
        }

        virtual void SerializeBytes(std::vector<std::byte>& bytes) const noexcept = 0;

        template <typename T>
        [[nodiscard]] uint32_t GetSize(T type) const noexcept
        {
            return sizeof(type);
        }

        [[nodiscard]] uint32_t GetSize(const std::string& s) const noexcept
        {
            return sizeof(uint32_t) + // size of string
                   static_cast<uint32_t>(s.size()) * sizeof(uint8_t);
        }

        [[nodiscard]] uint32_t GetSize(const std::vector<std::byte>& v) const noexcept
        {
            return sizeof(uint32_t) + // size of v
                   static_cast<uint32_t>(v.size()) * sizeof(std::byte);
        }
	};
}

#endif
