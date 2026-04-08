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

TEST_CASE("evaluate example from specification") {
    const std::string input =
        "evaluate\n"
        "2\n"
        "x y\n"
        "3 4\n"
        "x*x + y*2\n";

    REQUIRE(runCase(input) == "17");
}

TEST_CASE("evaluate supports decimal numbers") {
    const std::string input =
        "evaluate\n"
        "1\n"
        "x\n"
        "1.5\n"
        "x + 0.5\n";

    REQUIRE(runCase(input) == "2");
}

TEST_CASE("evaluate supports scientific notation") {
    const std::string input =
        "evaluate\n"
        "1\n"
        "x\n"
        "2\n"
        "1e2 + x\n";

    REQUIRE(runCase(input) == "102");
}

TEST_CASE("identifiers are case-insensitive") {
    const std::string input =
        "evaluate\n"
        "1\n"
        "X\n"
        "5\n"
        "x + X + x\n";

    REQUIRE(runCase(input) == "15");
}

TEST_CASE("derivative of polynomial is simplified") {
    const std::string input =
        "derivative\n"
        "1\n"
        "x\n"
        "10\n"
        "x^2\n";

    const std::string output = runCase(input);
    REQUIRE(output.find("x") != std::string::npos);
    REQUIRE(output.find("2") != std::string::npos);
}

TEST_CASE("evaluate_derivative computes value in point") {
    const std::string input =
        "evaluate_derivative\n"
        "1\n"
        "x\n"
        "3\n"
        "x^2\n";

    REQUIRE(runCase(input) == "6");
}

TEST_CASE("first declared variable is used for differentiation") {
    const std::string input =
        "evaluate_derivative\n"
        "2\n"
        "y x\n"
        "10 3\n"
        "x*y\n";

    REQUIRE(runCase(input) == "3");
}

TEST_CASE("unknown variable returns detailed error") {
    const std::string input =
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "x + z\n";

    const std::string output = runCase(input);
    REQUIRE(output.rfind("ERROR ", 0) == 0);
    REQUIRE(output.find("Unknown variable: z") != std::string::npos);
}

TEST_CASE("leading zero is rejected") {
    const std::string input =
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "012 + x\n";

    const std::string output = runCase(input);
    REQUIRE(output.rfind("ERROR ", 0) == 0);
    REQUIRE(output.find("leading zero") != std::string::npos);
}

TEST_CASE("sqrt domain error is reported") {
    const std::string input =
        "evaluate\n"
        "1\n"
        "x\n"
        "1\n"
        "sqrt(-1)\n";

    const std::string output = runCase(input);
    REQUIRE(output.rfind("ERROR ", 0) == 0);
    REQUIRE(output.find("sqrt") != std::string::npos);
}

TEST_CASE("variable name cannot shadow built-in function") {
    const std::string input =
        "evaluate\n"
        "1\n"
        "sin\n"
        "1\n"
        "sin + 2\n";

    const std::string output = runCase(input);
    REQUIRE(output.rfind("ERROR ", 0) == 0);
    REQUIRE(output.find("built-in function") != std::string::npos);
}