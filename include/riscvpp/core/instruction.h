//
// Created by Antonie Gabriel Belu on 21.05.2026.
//

#ifndef RISCVPP_INSTRUCTION_H
#define RISCVPP_INSTRUCTION_H

#include <riscvpp/core/register.h>
#include <cstdint>

namespace riscvpp {

    enum class Extension : uint8_t {
        INTEGER,
        MULTIPLY,
        FLOATING_POINT,
        COMPRESSED,
        ATOMIC,
        SYSTEM,
        UNKNOWN
    };

    class Instruction {
        uint32_t raw;
    public:
        constexpr explicit Instruction(const uint32_t bits) noexcept : raw(bits) {}

        [[nodiscard]] constexpr size_t size() const noexcept {
            if ((raw & 0x03) != 0x03) {
                return 2;
            }
            return 4;
        }

        [[nodiscard]] constexpr uint8_t opcode() const noexcept { return raw & 0x7F; }
        [[nodiscard]] constexpr uint8_t funct3() const noexcept { return (raw >> 12) & 0x07; }
        [[nodiscard]] constexpr uint8_t funct7() const noexcept { return (raw >> 25) & 0x7F; }
        [[nodiscard]] constexpr RegisterId rd() const noexcept { return static_cast<RegisterId>((raw >> 7) & 0x1F); }
        [[nodiscard]] constexpr RegisterId rs1() const noexcept { return static_cast<RegisterId>((raw >> 15) & 0x1F); }
        [[nodiscard]] constexpr RegisterId rs2() const noexcept { return static_cast<RegisterId>((raw >> 20) & 0x1F); }

        template <typename T>
        [[nodiscard]] constexpr T imm_i() const noexcept {
            const auto raw_i32 = static_cast<int32_t>(raw);
            return static_cast<T>(raw_i32 >> 20);
        }

        template <typename T>
        [[nodiscard]] constexpr T imm_s() const noexcept {
            const int32_t upper = static_cast<int32_t>(raw & 0xFE000000) >> 20;
            const auto lower = static_cast<int32_t>((raw >> 7) & 0x1F);
            return static_cast<T>(upper | lower);
        }

        template <typename T>
        [[nodiscard]] constexpr T imm_b() const noexcept {
            const auto raw_i32 = static_cast<int32_t>(raw);

            const int32_t upper = (static_cast<int32_t>(raw & 0x80000000) >> 19) | static_cast<int32_t>((raw & 0x7E000000) >> 20);
            const auto lower = static_cast<int32_t>((raw >> 7) & 0x1E);
            const int32_t bit11 = (raw_i32 & 0x80) << 4;

            const int32_t combined = upper | lower | bit11;
            return static_cast<T>(combined);
        }

        template <typename T>
        [[nodiscard]] constexpr T imm_u() const noexcept {
            return static_cast<T>(raw & 0xFFFFF000);
        }

        template <typename T>
        [[nodiscard]] constexpr T imm_j() const noexcept {
            const uint32_t bits_1_10 = (raw & 0x7FE00000) >> 20;
            const uint32_t bit11 = (raw & 0x00100000) >> 9;
            const uint32_t bit12_19 = raw & 0x000FF000;
            const int32_t combined =
                (static_cast<int32_t>(raw & 0x80000000) >> 11) |
                static_cast<int32_t>(bits_1_10) |
                static_cast<int32_t>(bit11) |
                static_cast<int32_t>(bit12_19);

            return static_cast<T>(combined);
        }

        [[nodiscard]] constexpr Extension extension() const noexcept {
            if (size() == 2) {
                return Extension::COMPRESSED;
            }

            switch (opcode()) {
                case 0x03:
                case 0x13:
                case 0x17:
                case 0x1B:
                case 0x23:
                case 0x37:
                case 0x63:
                case 0x67:
                case 0x6F:
                case 0x0F:
                    return Extension::INTEGER;

                case 0x33:
                case 0x3B:
                    if (funct7()) return Extension::MULTIPLY;
                    return Extension::INTEGER;

                case 0x2F:
                    return Extension::ATOMIC;

                case 0x07:
                case 0x27:
                case 0x43:
                case 0x47:
                case 0x4B:
                case 0x4F:
                case 0x53:
                    return Extension::FLOATING_POINT;

                case 0x73:
                    return Extension::SYSTEM;

                default:
                    return Extension::UNKNOWN;
            }
        }
    };
} // riscvpp

#endif //RISCVPP_INSTRUCTION_H