//
// Created by Antonie Gabriel Belu on 30/05/2026.
//

#include <riscvpp/core/instruction.h>
#include <iostream>
#include <string_view>
#include <vector>
#include <sstream>

// Global or file-static vector to accumulate failures
std::vector<std::string> failure_logs;

template <typename T>
bool assert_equal(const T& actual, const T& expected, const std::string_view test_name) {
    if (actual != expected) {
        // Capture the error message into a stringstream instead of printing immediately
        std::ostringstream oss;
        oss << "[FAIL] " << test_name << "\n"
            << "Expected: " << expected << "\n"
            << "Actual:   " << actual << "\n";

        failure_logs.push_back(oss.str());
        return false;
    }

    std::cout << "[PASS] " << test_name << "\n";
    return true;
}

void test_instruction_size(bool& all_passed) {
    constexpr riscvpp::Instruction instr_c1(0x00000000);
    constexpr riscvpp::Instruction instr_c2(0x00000002);
    constexpr riscvpp::Instruction instr_32(0x00000003);

    all_passed &= assert_equal(instr_c1.size(), static_cast<size_t>(2), "Instruction size 2 (bits 00)");
    all_passed &= assert_equal(instr_c2.size(), static_cast<size_t>(2), "Instruction size 2 (bits 10)");
    all_passed &= assert_equal(instr_32.size(), static_cast<size_t>(4), "Instruction size 4 (bits 11)");
}

void test_basic_fields(bool& all_passed) {
    constexpr uint32_t bits = 0xB4007033;
    constexpr riscvpp::Instruction instr(bits);

    all_passed &= assert_equal(static_cast<int>(instr.opcode()), 0x33, "Opcode field extraction");
    all_passed &= assert_equal(static_cast<int>(instr.funct3()), 0x07, "Funct3 field extraction");
    all_passed &= assert_equal(static_cast<int>(instr.funct7()), 0x5A, "Funct7 field extraction");
}

void test_register_fields(bool& all_passed) {
    constexpr uint32_t bits = 0x00C58500;
    constexpr riscvpp::Instruction instr(bits);

    all_passed &= assert_equal(static_cast<int>(instr.rd()), 10, "rd register index extraction");
    all_passed &= assert_equal(static_cast<int>(instr.rs1()), 11, "rs1 register index extraction");
    all_passed &= assert_equal(static_cast<int>(instr.rs2()), 12, "rs2 register index extraction");
}

void test_imm_i_extraction(bool& all_passed) {
    constexpr riscvpp::Instruction instr_pos(0x12300000);
    all_passed &= assert_equal(instr_pos.imm_i<int32_t>(), 0x123, "I-type positive immediate");

    constexpr riscvpp::Instruction instr_neg(0xFFF00000);
    all_passed &= assert_equal(instr_neg.imm_i<int32_t>(), -1, "I-type negative sign-extended immediate");
}

void test_imm_s_extraction(bool& all_passed) {
    constexpr riscvpp::Instruction instr_pos(0x02000280);
    all_passed &= assert_equal(instr_pos.imm_s<int32_t>(), 0x025, "S-type positive immediate");

    constexpr riscvpp::Instruction instr_neg(0xFE000000);
    all_passed &= assert_equal(instr_neg.imm_s<int32_t>(), -32, "S-type negative sign-extended immediate");
}

void test_imm_b_extraction(bool& all_passed) {
    constexpr riscvpp::Instruction instr_pos(0x00000800);
    all_passed &= assert_equal(instr_pos.imm_b<int32_t>(), 16, "B-type positive immediate");

    constexpr riscvpp::Instruction instr_neg(0xFE000880);
    all_passed &= assert_equal(instr_neg.imm_b<int32_t>(), -16, "B-type negative sign-extended immediate");
}

void test_imm_u_extraction(bool& all_passed) {
    constexpr riscvpp::Instruction instr(0x12345678);
    all_passed &= assert_equal(instr.imm_u<uint32_t>(), static_cast<uint32_t>(0x12345000), "U-type clears lower 12 bits");
}

void test_imm_j_extraction(bool& all_passed) {
    constexpr riscvpp::Instruction instr_pos(0x00100000);
    all_passed &= assert_equal(instr_pos.imm_j<int32_t>(), 2048, "J-type positive immediate");

    constexpr riscvpp::Instruction instr_neg(0xFFFFF000);
    all_passed &= assert_equal(instr_neg.imm_j<int32_t>(), -2, "J-type negative immediate");
}

void test_real_riscv_immediates(bool& all_passed) {
    constexpr riscvpp::Instruction addi(0xFFB58513);
    all_passed &= assert_equal(addi.imm_i<int32_t>(), -5, "I-type: addi correctly decodes -5");

    constexpr riscvpp::Instruction sw(0x00A5A423);
    all_passed &= assert_equal(sw.imm_s<int32_t>(), 8, "S-type: sw correctly decodes offset 8");

    constexpr riscvpp::Instruction bne(0xFEA51CE3);
    all_passed &= assert_equal(bne.imm_b<int32_t>(), -8, "B-type: bne correctly decodes offset -8");

    constexpr riscvpp::Instruction jal(0x801FF06F);
    all_passed &= assert_equal(jal.imm_j<int32_t>(), -2048, "J-type: jal correctly decodes offset -2048");
}

void test_extension_detection(bool& all_passed) {
    constexpr riscvpp::Instruction instr_comp(0x00000000);
    all_passed &= assert_equal(static_cast<int>(instr_comp.extension()), static_cast<int>(riscvpp::Extension::COMPRESSED), "Extension: COMPRESSED");

    constexpr riscvpp::Instruction instr_int(0x00000013);
    all_passed &= assert_equal(static_cast<int>(instr_int.extension()), static_cast<int>(riscvpp::Extension::INTEGER), "Extension: INTEGER (0x13)");

    constexpr riscvpp::Instruction instr_op_int(0x00000033);
    constexpr riscvpp::Instruction instr_op_mul(0x02000033);
    all_passed &= assert_equal(static_cast<int>(instr_op_int.extension()), static_cast<int>(riscvpp::Extension::INTEGER), "Extension: INTEGER (0x33, funct7=0)");
    all_passed &= assert_equal(static_cast<int>(instr_op_mul.extension()), static_cast<int>(riscvpp::Extension::MULTIPLY), "Extension: MULTIPLY (0x33, funct7!=0)");

    constexpr riscvpp::Instruction instr_atomic(0x0000002F);
    all_passed &= assert_equal(static_cast<int>(instr_atomic.extension()), static_cast<int>(riscvpp::Extension::ATOMIC), "Extension: ATOMIC");

    constexpr riscvpp::Instruction instr_fp(0x00000053);
    all_passed &= assert_equal(static_cast<int>(instr_fp.extension()), static_cast<int>(riscvpp::Extension::FLOATING_POINT), "Extension: FLOATING_POINT");

    constexpr riscvpp::Instruction instr_sys(0x00000073);
    all_passed &= assert_equal(static_cast<int>(instr_sys.extension()), static_cast<int>(riscvpp::Extension::SYSTEM), "Extension: SYSTEM");

    constexpr riscvpp::Instruction instr_unk(0x0000007F);
    all_passed &= assert_equal(static_cast<int>(instr_unk.extension()), static_cast<int>(riscvpp::Extension::UNKNOWN), "Extension: UNKNOWN");
}

int main() {
    std::cout << "Running Instruction Tests...\n\n";
    bool all_passed = true;

    test_instruction_size(all_passed);
    test_basic_fields(all_passed);
    test_register_fields(all_passed);
    test_imm_i_extraction(all_passed);
    test_imm_s_extraction(all_passed);
    test_imm_b_extraction(all_passed);
    test_imm_u_extraction(all_passed);
    test_imm_j_extraction(all_passed);
    test_extension_detection(all_passed);
    test_real_riscv_immediates(all_passed);

    std::cout << std::endl;

    // Dump all accumulated failure logs to std::cerr at the very end
    if (!failure_logs.empty()) {
        std::cerr << "--- TEST SUITE FAILURES ---\n";
        for (const auto& log : failure_logs) {
            std::cerr << log << "\n";
        }
        std::cout << "Result: One or more tests in this suite FAILED.\n\n";
        return 1;
    }

    std::cout << "Result: All tests in this suite PASSED.\n\n";
    return 0;
}