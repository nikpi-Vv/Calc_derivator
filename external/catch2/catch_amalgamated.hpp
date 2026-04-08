#pragma once

#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace mini_catch {

struct TestCase {
    std::string name;
    std::function<void()> func;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(std::string name, std::function<void()> func) {
        registry().push_back(TestCase{std::move(name), std::move(func)});
    }
};

class AssertionFailure : public std::exception {
public:
    explicit AssertionFailure(std::string message) : message_(std::move(message)) {}
    const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

inline void require(bool condition, const char* expr, const char* file, int line) {
    if (!condition) {
        std::ostringstream out;
        out << file << ':' << line << ": REQUIRE(" << expr << ") failed";
        throw AssertionFailure(out.str());
    }
}

inline void requireNear(double lhs, double rhs, double eps, const char* lhsExpr, const char* rhsExpr, const char* file, int line) {
    if (std::fabs(lhs - rhs) > eps) {
        std::ostringstream out;
        out << file << ':' << line << ": REQUIRE_NEAR(" << lhsExpr << ", " << rhsExpr
            << ") failed: " << lhs << " vs " << rhs << ", eps=" << eps;
        throw AssertionFailure(out.str());
    }
}

inline int runAll() {
    int failed = 0;
    for (const auto& test : registry()) {
        try {
            test.func();
            std::cout << "[OK] " << test.name << '\n';
        } catch (const std::exception& e) {
            ++failed;
            std::cout << "[FAIL] " << test.name << "\n  " << e.what() << '\n';
        } catch (...) {
            ++failed;
            std::cout << "[FAIL] " << test.name << "\n  Unknown exception\n";
        }
    }

    std::cout << "\nTotal: " << registry().size() << ", failed: " << failed << '\n';
    return failed == 0 ? 0 : 1;
}

}  // namespace mini_catch

#define MC_CONCAT_IMPL(a, b) a##b
#define MC_CONCAT(a, b) MC_CONCAT_IMPL(a, b)

#define TEST_CASE(name) \
    static void MC_CONCAT(test_func_, __LINE__)(); \
    static mini_catch::Registrar MC_CONCAT(test_reg_, __LINE__)(name, MC_CONCAT(test_func_, __LINE__)); \
    static void MC_CONCAT(test_func_, __LINE__)()

#define REQUIRE(expr) mini_catch::require((expr), #expr, __FILE__, __LINE__)
#define REQUIRE_NEAR(lhs, rhs, eps) mini_catch::requireNear((lhs), (rhs), (eps), #lhs, #rhs, __FILE__, __LINE__)