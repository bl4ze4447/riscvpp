//
// Created by Antonie Gabriel Belu on 21.05.2026.
//

#include <riscvpp/core/register.h>
#include <iostream>
#include <string_view>
#include <stdexcept>

template <typename T>
bool assert_equal(const T& actual, const T& expected, const std::string_view test_name) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << test_name << "\n"
                  << "Expected: " << expected << "\n"
                  << "Actual:   " << actual << "\n\n";
        return false;
    }

    std::cout << "[PASS] " << test_name << "\n";
    return true;
}

// Verifies that both x0 and standard registers start cleared to zero on initialization
void test_initial_state(const riscvpp::Registers<int>& regs, bool& all_passed) {
    all_passed &= assert_equal(regs[0], 0, "x0 initialized to zero");
    all_passed &= assert_equal(regs[5], 0, "x5 initialized to zero");
}

// Verifies that a normal general-purpose register successfully retains written values
void test_standard_read_write(riscvpp::Registers<int>& regs, const riscvpp::Registers<int>& regs_view, bool& all_passed) {
    regs[5].set(0xABCDE);
    all_passed &= assert_equal(regs_view[5], 0xABCDE, "Standard write and read on x5");
}

// Verifies that writes to x0 are safely discarded by the proxy and always evaluate to zero
void test_x0_immutable_constraint(riscvpp::Registers<int>& regs, const riscvpp::Registers<int>& regs_view, bool& all_passed) {
    regs[0].set(999);
    all_passed &= assert_equal(regs_view[0], 0, "Hardwired x0 discards writes");
}

// Verifies that distinct memory cells don't spill modifications over to neighboring indices
void test_register_isolation(riscvpp::Registers<int>& regs, const riscvpp::Registers<int>& regs_view, bool& all_passed) {
    regs[6].set(42);
    regs[7].set(84);
    all_passed &= assert_equal(regs_view[6], 42, "Register x6 is isolated");
    all_passed &= assert_equal(regs_view[7], 84, "Register x7 is isolated");
}

// Verifies that sequential writes to the x0 proxy do not store or accumulate residual dirty states
void test_x0_consecutive_writes(riscvpp::Registers<int>& regs, const riscvpp::Registers<int>& regs_view, bool& all_passed) {
    regs[0].set(111);
    regs[0].set(222);
    regs[0].set(333);
    all_passed &= assert_equal(regs_view[0], 0, "Hardwired x0 remains zero after consecutive writes");
}

// Verifies that array-bound safety policies cleanly abort execution upon malicious out-of-bounds indexing
void test_out_of_bounds_protection(riscvpp::Registers<int>& regs, const riscvpp::Registers<int>& regs_view, bool& all_passed) {
    bool threw_on_write = false;
    bool threw_on_read = false;

    try {
        regs[32].set(5);
    } catch (const std::out_of_range&) {
        threw_on_write = true;
    }

    try {
        (void)regs_view[32];
    } catch (const std::out_of_range&) {
        threw_on_read = true;
    }

    all_passed &= assert_equal(threw_on_write, true, "Out of bounds write throws out_of_range");
    all_passed &= assert_equal(threw_on_read, true, "Out of bounds read throws out_of_range");
}

int main() {
    std::cout << "Running Register Tests...\n";
    bool all_passed = true;

    riscvpp::Registers<int> regs;
    const auto& regs_view = regs;

    test_initial_state(regs_view, all_passed);
    test_standard_read_write(regs, regs_view, all_passed);
    test_x0_immutable_constraint(regs, regs_view, all_passed);
    test_register_isolation(regs, regs_view, all_passed);
    test_x0_consecutive_writes(regs, regs_view, all_passed);
    test_out_of_bounds_protection(regs, regs_view, all_passed);

    if (!all_passed) {
        return 1;
    }

    std::cout << "Result: All tests in this suite PASSED.\n\n";
    return 0;
}