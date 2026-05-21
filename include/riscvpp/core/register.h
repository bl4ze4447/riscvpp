//
// Created by Antonie Gabriel Belu on 21.05.2026.
//

#ifndef RISCVPP_REGISTER_H
#define RISCVPP_REGISTER_H

#include <string_view>
#include <array>
#include <stdexcept>
#include <cstdint>

namespace riscvpp {

    enum class RegisterId : uint8_t {
        ZERO = 0, RA,  SP,  GP,  TP,  T0,  T1,  T2,
        S0_FP,    S1,  A0,  A1,  A2,  A3,  A4,  A5,
        A6,       A7,  S2,  S3,  S4,  S5,  S6,  S7,
        S8,       S9,  S10, S11, T3,  T4,  T5,  T6
    };

    enum class FloatRegisterId : uint8_t {
        FT0 = 0, FT1,  FT2,  FT3,  FT4,  FT5,  FT6,  FT7,
        FS0,     FS1,  FA0,  FA1,  FA2,  FA3,  FA4,  FA5,
        FA6,     FA7,  FS2,  FS3,  FS4,  FS5,  FS6,  FS7,
        FS8,     FS9,  FS10, FS11, FT8,  FT9,  FT10, FT11
    };

    template<typename T>
    class Register {
        T value;

    public:
        constexpr explicit Register(T value = 0) noexcept : value(value) {}

        constexpr T get() const noexcept { return value; }
        constexpr void set(T val) noexcept { value = val; }
        constexpr T& get_ref() noexcept { return value; }

        static constexpr std::string_view get_name(RegisterId id) noexcept {
            return get_gpr_name(static_cast<size_t>(id));
        }

        static constexpr std::string_view get_name(FloatRegisterId id) noexcept {
            return get_fpr_name(static_cast<size_t>(id));
        }

    private:
        // General purpose registers
        static constexpr std::string_view get_gpr_name(const size_t index) noexcept {
            static constexpr std::array<std::string_view, 32> names = {
                "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
                "s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
                "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
                "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
            };

            if (index >= names.size()) [[unlikely]] return "unknown";
            return names[index];
        }

        // Floating-point registers
        static constexpr std::string_view get_fpr_name(const size_t index) noexcept {
            static constexpr std::array<std::string_view, 32> names = {
                "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
                "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5",
                "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7",
                "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"
            };

            if (index >= names.size()) [[unlikely]] return "unknown";
            return names[index];
        }
    };

    template<typename T>
    class Registers {
        std::array<Register<T>, 32> registers;

    public:
        class WriteHandle {
            Register<T> *target;
        public:
            constexpr explicit WriteHandle(Register<T> *target) noexcept : target(target) {}

            WriteHandle(const WriteHandle&) = default;
            WriteHandle& operator=(const WriteHandle&) = delete;

            constexpr void set(T val) const noexcept {
                target->set(val);
            }
        };

        WriteHandle operator[](size_t index) {
            if (index >= registers.size()) [[unlikely]] {
                throw std::out_of_range("Register index out of range!");
            }

            if (index == 0) [[unlikely]]  {
                static Register<T> dummy_zero{0};
                dummy_zero.set(0);
                return WriteHandle(&dummy_zero);
            }

            return WriteHandle(&registers[index]);
        }

        constexpr T operator[](size_t index) const {
            if (index >= registers.size()) [[unlikely]] {
                throw std::out_of_range("Register index out of range!");
            }

            if (index == 0) {
                return 0;
            }
            return registers[index].get();
        }
    };
} // riscvpp

#endif //RISCVPP_REGISTER_H