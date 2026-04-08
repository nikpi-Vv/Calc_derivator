#include <sstream>
#include <string>

#include "app.hpp"
#include "catch2/catch_amalgamated.hpp"

namespace {
std::string runCaseHard(const std::string& input) {
    std::istringstream in(input);
    std::ostringstream out;
    runApplication(in, out);
    return out.str();
}
    double runCaseHardDouble(const std::string& input) {
    return std::stod(runCaseHard(input));
}
}

// =========================
// HARD EVALUATE
// =========================

TEST_CASE("HARD EVAL 01: weird spaces around everything") {
    REQUIRE(runCaseHard(
        "evaluate\n"
        "2\n"
        "x      y\n"
        "2      3\n"
        "   x   +    y *  2   \n"
    ) == "8");
}

TEST_CASE("HARD EVAL 02: case-insensitive variables and functions together") {
    REQUIRE_NEAR(runCaseHardDouble(
        "evaluate\n"
        "1\n"
        "X\n"
        "1\n"
        "sIn(X) + CoS(x)\n"
    ), 1.38177329067604, 1e-12);
}

TEST_CASE("HARD EVAL 03: nested functions") {
    REQUIRE_NEAR(runCaseHardDouble(
        "evaluate\n"
        "1\n"
        "x\n"
        "2\n"
        "sin(cos(log(x)))\n"
    ), 0.695588636223164, 1e-12);
}

TEST_CASE("HARD EVAL 04: sqrt(x^2) at negative x") {
    REQUIRE(runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "-3\n"
        "sqrt(x^2)\n"
    ) == "3");
}

TEST_CASE("HARD EVAL 05: exponent notation with negative exponent") {
    REQUIRE(runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "2.5e-1 + 0.75\n"
    ) == "1");
}

TEST_CASE("HARD EVAL 06: right associative powers deeper") {
    REQUIRE(runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "3^2^2\n"
    ) == "81");
}

TEST_CASE("HARD EVAL 07: power with unary minus on exponent") {
    REQUIRE(runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "4^-1\n"
    ) == "0.25");
}

TEST_CASE("HARD EVAL 08: variable expression with all operations") {
    REQUIRE_NEAR(runCaseHardDouble(
        "evaluate\n"
        "3\n"
        "x y z\n"
        "2 5 4\n"
        "(x + y) * z - y / x + x^z\n"
    ), 41.5, 1e-12);
}

TEST_CASE("HARD EVAL 09: log of exp") {
    REQUIRE(runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "2\n"
        "log(exp(x))\n"
    ) == "2");
}

TEST_CASE("HARD EVAL 10: repeated unary operators before variable") {
    REQUIRE(runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "5\n"
        "---x\n"
    ) == "-5");
}

// =========================
// HARD EVALUATE_DERIVATIVE
// =========================

TEST_CASE("HARD DE 01: derivative of sin(x*x)") {
    REQUIRE(runCaseHard(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "sin(x*x)\n"
    ) == "1.08060461173628");
}

TEST_CASE("HARD DE 02: derivative of log(x^2)") {
    REQUIRE(runCaseHard(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "3\n"
        "log(x^2)\n"
    ) == "0.666666666666667");
}

TEST_CASE("HARD DE 03: derivative of sqrt(sin(x))") {
    REQUIRE_NEAR(runCaseHardDouble(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "sqrt(sin(x))\n"
    ), 0.294501324687269, 1e-12);
}

TEST_CASE("HARD DE 04: derivative only by first variable") {
    REQUIRE(runCaseHard(
        "evaluate_derivative\n"
        "3\n"
        "z y x\n"
        "2 10 100\n"
        "x*z + y\n"
    ) == "100");
}

TEST_CASE("HARD DE 05: derivative of variable-independent expression") {
    REQUIRE(runCaseHard(
        "evaluate_derivative\n"
        "2\n"
        "x y\n"
        "3 4\n"
        "y^3 + sin(y)\n"
    ) == "0");
}

TEST_CASE("HARD DE 06: derivative of x^x at x=2") {
    REQUIRE_NEAR(runCaseHardDouble(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "2\n"
        "x^x\n"
    ), 6.77258872223978, 1e-12);
}

TEST_CASE("HARD DE 07: derivative of exp(log(x))") {
    REQUIRE(runCaseHard(
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "5\n"
        "exp(log(x))\n"
    ) == "1");
}

TEST_CASE("HARD DE 08: derivative of sin(x*y) by x") {
    REQUIRE(runCaseHard(
        "evaluate_derivative\n"
        "2\n"
        "x y\n"
        "1 2\n"
        "sin(x*y)\n"
    ) == "-0.832293673094285");
}

// =========================
// HARD DERIVATIVE STRING
// =========================

TEST_CASE("HARD SD 01: derivative of constant still 0") {
    REQUIRE(runCaseHard(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "42\n"
    ) == "0");
}

TEST_CASE("HARD SD 02: derivative of x^2 contains x and 2") {
    const std::string out = runCaseHard(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "x^2\n"
    );
    REQUIRE(out.find("x") != std::string::npos);
    REQUIRE(out.find("2") != std::string::npos);
}

TEST_CASE("HARD SD 03: derivative of sin(x) contains cos") {
    const std::string out = runCaseHard(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "sin(x)\n"
    );
    REQUIRE(out.find("cos") != std::string::npos);
}

TEST_CASE("HARD SD 04: derivative of cos(x) contains sin") {
    const std::string out = runCaseHard(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "cos(x)\n"
    );
    REQUIRE(out.find("sin") != std::string::npos);
}

TEST_CASE("HARD SD 05: derivative of log(x) contains x") {
    const std::string out = runCaseHard(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "log(x)\n"
    );
    REQUIRE(out.find("x") != std::string::npos);
}

TEST_CASE("HARD SD 06: derivative of sqrt(x) contains sqrt") {
    const std::string out = runCaseHard(
        "derivative\n"
        "1\n"
        "x\n"
        "1\n"
        "sqrt(x)\n"
    );
    REQUIRE(out.find("sqrt") != std::string::npos);
}

// =========================
// HARD ERRORS
// =========================

TEST_CASE("HARD ERR 01: unknown command") {
    const std::string out = runCaseHard(
        "calc\n"
        "1\n"
        "x\n"
        "1\n"
        "x+1\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 02: unknown variable") {
    const std::string out = runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "x + y\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 04: missing closing parenthesis") {
    const std::string out = runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "sin(x\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 05: extra closing parenthesis") {
    const std::string out = runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "x+1)\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 06: bad number with leading zero") {
    const std::string out = runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "002.5 + x\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 07: sqrt negative domain") {
    const std::string out = runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "sqrt(-4)\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 08: log negative domain") {
    const std::string out = runCaseHard(
        "evaluate\n"
        "1\n"
        "x\n"
        "0\n"
        "log(-1)\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 09: built-in function name as variable") {
    const std::string out = runCaseHard(
        "evaluate\n"
        "1\n"
        "sqrt\n"
        "2\n"
        "sqrt + 1\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}

TEST_CASE("HARD ERR 10: no variables for derivative") {
    const std::string out = runCaseHard(
        "derivative\n"
        "0\n"
        "\n"
        "x^2\n"
    );
    REQUIRE(out.rfind("ERROR ", 0) == 0);
}