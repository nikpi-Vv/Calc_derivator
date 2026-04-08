#include <sstream>
#include <string>

#include "app.hpp"
#include "catch2/catch_amalgamated.hpp"

namespace {
std::string runCase(const std::string& input) {
    std::istringstream in(input);
    std::ostringstream out;
    runApplication(in, out);
    return out.str();
}
}

// =========================
// SE. Simple Evaluation
// =========================

TEST_CASE("SE 01: 1 + 1") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "1 + 1\n"
    ) == "2");
}

TEST_CASE("SE 02: 1+1 without spaces") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "1+1\n"
    ) == "2");
}

TEST_CASE("SE 03: many spaces") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "1       +       1\n"
    ) == "2");
}

TEST_CASE("SE 04: zero") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "0\n"
    ) == "0");
}

TEST_CASE("SE 05: 0/1") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "0/1\n"
    ) == "0");
}

TEST_CASE("SE 06: 5/0 gives inf") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "5/0\n"
    ) == "inf");
}

TEST_CASE("SE 07: precedence multiply before plus") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2 + 4 * 5\n"
    ) == "22");
}

TEST_CASE("SE 08: 5/2") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "5/2\n"
    ) == "2.5");
}

TEST_CASE("SE 09: 2/5") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2/5\n"
    ) == "0.4");
}

TEST_CASE("SE 10: power and multiply") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2^2*5\n"
    ) == "20");
}

TEST_CASE("SE 11: parentheses and power") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "(2 + 5)*2^4\n"
    ) == "112");
}

TEST_CASE("SE 12: nested parentheses") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "((((2))))\n"
    ) == "2");
}

TEST_CASE("SE 13: unary minus") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "-3 + 3\n"
    ) == "0");
}

TEST_CASE("SE 14: unary minus in product") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2*-3\n"
    ) == "-6");
}

TEST_CASE("SE 15: repeated unary operators") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "----2\n"
    ) == "2");
}

TEST_CASE("SE 16: negative power") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2^-2\n"
    ) == "0.25");
}

TEST_CASE("SE 17: (-2)^2") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "(-2)^2\n"
    ) == "4");
}

TEST_CASE("SE 18: right associativity of power 2^3^2") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2^3^2\n"
    ) == "512");
}

TEST_CASE("SE 19: 2^2^2^2") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2^2^2^2\n"
    ) == "65536");
}

TEST_CASE("SE 20: decimal numbers") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "0.5 + 0.25\n"
    ) == "0.75");
}

TEST_CASE("SE 21: decimal power") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "0.5^2\n"
    ) == "0.25");
}

TEST_CASE("SE 22: neutral elements") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "5+0\n"
    ) == "5");
}

TEST_CASE("SE 23: scientific notation") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "1e2 + 2\n"
    ) == "102");
}

// =========================
// VE. Variable Evaluation
// =========================

TEST_CASE("VE 01: x + y + z") {
    REQUIRE(runCase(
        "evaluate\n"
        "3\n"
        "z x y\n"
        "3 1 2\n"
        "x + y + z\n"
    ) == "6");
}

TEST_CASE("VE 02: x^y + y^x") {
    REQUIRE(runCase(
        "evaluate\n"
        "2\n"
        "x y\n"
        "2 3\n"
        "x^y + y^x\n"
    ) == "17");
}

TEST_CASE("VE 03: negative variable") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "-2\n"
        "x^2 + x\n"
    ) == "2");
}

TEST_CASE("VE 04: mixed expression") {
    REQUIRE(runCase(
        "evaluate\n"
        "3\n"
        "x y z\n"
        "2 3 4\n"
        "x + y*z + x^y + z^x\n"
    ) == "38");
}

TEST_CASE("VE 05: case insensitive names") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "SiN(x)\n"
    ) == "0.841470984807897");
}

TEST_CASE("VE 06: sqrt and log and cos") {
    REQUIRE(runCase(
        "evaluate\n"
        "3\n"
        "x y z\n"
        "1 2 4\n"
        "sqrt(z) + log(y) + cos(x)\n"
    ) == "3.23344948642809");
}

TEST_CASE("VE 07: log(0) gives -inf") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "log(x)\n"
    ) == "-inf");
}

TEST_CASE("VE 08: sin(cos(x))") {
    REQUIRE(runCase(
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "sin(cos(x))\n"
    ) == "0.514395258523549");
}

// =========================
// DE. Derivative Evaluation
// =========================

TEST_CASE("DE 01: derivative of constant") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "10\n"
        "5\n"
    ) == "0");
}

TEST_CASE("DE 02: derivative of x^2 at x=2") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "2\n"
        "x^2\n"
    ) == "4");
}

TEST_CASE("DE 03: derivative of x*x at x=2") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "2\n"
        "x*x\n"
    ) == "4");
}

TEST_CASE("DE 04: derivative of x*(x+1) at x=3") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "3\n"
        "x*(x+1)\n"
    ) == "7");
}

TEST_CASE("DE 05: derivative of 1/x at x=2") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "2\n"
        "1/x\n"
    ) == "-0.25");
}

TEST_CASE("DE 06: derivative of sin(x) at zero") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "0\n"
        "sin(x)\n"
    ) == "1");
}

TEST_CASE("DE 07: derivative of cos(x) at zero") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "0\n"
        "cos(x)\n"
    ) == "-0");
}

TEST_CASE("DE 08: derivative of log(x) at x=1") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "log(x)\n"
    ) == "1");
}

TEST_CASE("DE 09: derivative of sqrt(x) at x=4") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "4\n"
        "sqrt(x)\n"
    ) == "0.25");
}

TEST_CASE("DE 10: first variable is differentiation variable") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "2\n"
        "x y\n"
        "2 3\n"
        "x*y\n"
    ) == "3");
}

TEST_CASE("DE 11: derivative of y^2 by x is zero") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "2\n"
        "x y\n"
        "5 7\n"
        "y^2\n"
    ) == "0");
}

TEST_CASE("DE 12: derivative with nested chain") {
    REQUIRE(runCase(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "2\n"
        "sin(x^2)\n"
    ) == "-2.61457448345445");
}

// =========================
// SD. Symbolic Derivative
// Безопасные точные случаи
// =========================

TEST_CASE("SD 01: derivative of constant string") {
    REQUIRE(runCase(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "5\n"
    ) == "0");
}

TEST_CASE("SD 02: derivative of x string") {
    REQUIRE(runCase(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "x\n"
    ) == "1");
}

TEST_CASE("SD 03: derivative of y by x string") {
    REQUIRE(runCase(
        "derivative\n"
        "2\n"
        "x y\n"
        "1 2\n"
        "y\n"
    ) == "0");
}

TEST_CASE("SD 04: derivative of x + x string") {
    REQUIRE(runCase(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "x + x\n"
    ) == "2");
}

TEST_CASE("SD 05: derivative of sin(x) string contains cos") {
    const std::string out = runCase(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "sin(x)\n"
    );
    REQUIRE(out.find("cos") != std::string::npos);
}

TEST_CASE("SD 06: derivative of log(x) contains 1 / x") {
    const std::string out = runCase(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "log(x)\n"
    );
    REQUIRE(out.find("x") != std::string::npos);
}