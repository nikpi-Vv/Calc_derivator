#include "differentiator.hpp"

#include <memory>
#include <string>
#include <utility>

NodePtr Differentiator::differentiate(const Node& node, const std::string& variable) const {
    if (const auto* number = dynamic_cast<const NumberNode*>(&node)) {
        (void)number;
        return makeNumber("0");
    }

    if (const auto* var = dynamic_cast<const VariableNode*>(&node)) {
        if (var->name() == variable) {
            return makeNumber("1");
        }
        return makeNumber("0");
    }

    if (const auto* unary = dynamic_cast<const UnaryOpNode*>(&node)) {
        NodePtr dOperand = differentiate(*unary->operand(), variable);

        switch (unary->op()) {
            case UnaryOpType::Plus:
                return makeUnary(UnaryOpType::Plus, std::move(dOperand));
            case UnaryOpType::Minus:
                return makeUnary(UnaryOpType::Minus, std::move(dOperand));
        }

        throw DiffError("Unknown unary operator");
    }

    if (const auto* binary = dynamic_cast<const BinaryOpNode*>(&node)) {
        switch (binary->op()) {
            case BinaryOpType::Add:
                return makeBinary(
                    BinaryOpType::Add,
                    differentiate(*binary->left(), variable),
                    differentiate(*binary->right(), variable)
                );

            case BinaryOpType::Subtract:
                return makeBinary(
                    BinaryOpType::Subtract,
                    differentiate(*binary->left(), variable),
                    differentiate(*binary->right(), variable)
                );

            case BinaryOpType::Multiply: {
                // (u * v)' = u' * v + u * v'
                NodePtr leftPart = makeBinary(
                    BinaryOpType::Multiply,
                    differentiate(*binary->left(), variable),
                    clone(*binary->right())
                );

                NodePtr rightPart = makeBinary(
                    BinaryOpType::Multiply,
                    clone(*binary->left()),
                    differentiate(*binary->right(), variable)
                );

                return makeBinary(
                    BinaryOpType::Add,
                    std::move(leftPart),
                    std::move(rightPart)
                );
            }

            case BinaryOpType::Divide: {
                // (u / v)' = (u' * v - u * v') / v^2
                NodePtr numeratorLeft = makeBinary(
                    BinaryOpType::Multiply,
                    differentiate(*binary->left(), variable),
                    clone(*binary->right())
                );

                NodePtr numeratorRight = makeBinary(
                    BinaryOpType::Multiply,
                    clone(*binary->left()),
                    differentiate(*binary->right(), variable)
                );

                NodePtr numerator = makeBinary(
                    BinaryOpType::Subtract,
                    std::move(numeratorLeft),
                    std::move(numeratorRight)
                );

                NodePtr denominator = makeBinary(
                    BinaryOpType::Power,
                    clone(*binary->right()),
                    makeNumber("2")
                );

                return makeBinary(
                    BinaryOpType::Divide,
                    std::move(numerator),
                    std::move(denominator)
                );
            }

            case BinaryOpType::Power: {
                // Общая формула:
                // (u^v)' = u^v * (v' * log(u) + v * u'/u)

                NodePtr uPowV = makeBinary(
                    BinaryOpType::Power,
                    clone(*binary->left()),
                    clone(*binary->right())
                );

                NodePtr firstTerm = makeBinary(
                    BinaryOpType::Multiply,
                    differentiate(*binary->right(), variable),
                    makeFunction("log", clone(*binary->left()))
                );

                NodePtr secondTerm = makeBinary(
                    BinaryOpType::Multiply,
                    clone(*binary->right()),
                    makeBinary(
                        BinaryOpType::Divide,
                        differentiate(*binary->left(), variable),
                        clone(*binary->left())
                    )
                );

                NodePtr bracket = makeBinary(
                    BinaryOpType::Add,
                    std::move(firstTerm),
                    std::move(secondTerm)
                );

                return makeBinary(
                    BinaryOpType::Multiply,
                    std::move(uPowV),
                    std::move(bracket)
                );
            }
        }

        throw DiffError("Unknown binary operator");
    }

    if (const auto* call = dynamic_cast<const FunctionCallNode*>(&node)) {
        return differentiateFunction(*call, variable);
    }

    throw DiffError("Unknown AST node");
}

NodePtr Differentiator::differentiateFunction(const FunctionCallNode& call, const std::string& variable) const {
    const std::string& name = call.function_name();
    NodePtr du = differentiate(*call.argument(), variable);

    if (name == "sin") {
        // (sin u)' = cos(u) * u'
        return makeBinary(
            BinaryOpType::Multiply,
            makeFunction("cos", clone(*call.argument())),
            std::move(du)
        );
    }

    if (name == "cos") {
        // (cos u)' = -sin(u) * u'
        return makeBinary(
            BinaryOpType::Multiply,
            makeUnary(
                UnaryOpType::Minus,
                makeFunction("sin", clone(*call.argument()))
            ),
            std::move(du)
        );
    }

    if (name == "tan") {
        // (tan u)' = u' / cos(u)^2
        return makeBinary(
            BinaryOpType::Divide,
            std::move(du),
            makeBinary(
                BinaryOpType::Power,
                makeFunction("cos", clone(*call.argument())),
                makeNumber("2")
            )
        );
    }

    if (name == "asin") {
        // (asin u)' = u' / sqrt(1 - u^2)
        return makeBinary(
            BinaryOpType::Divide,
            std::move(du),
            makeFunction(
                "sqrt",
                makeBinary(
                    BinaryOpType::Subtract,
                    makeNumber("1"),
                    makeBinary(
                        BinaryOpType::Power,
                        clone(*call.argument()),
                        makeNumber("2")
                    )
                )
            )
        );
    }

    if (name == "acos") {
        // (acos u)' = -u' / sqrt(1 - u^2)
        return makeBinary(
            BinaryOpType::Divide,
            makeUnary(UnaryOpType::Minus, std::move(du)),
            makeFunction(
                "sqrt",
                makeBinary(
                    BinaryOpType::Subtract,
                    makeNumber("1"),
                    makeBinary(
                        BinaryOpType::Power,
                        clone(*call.argument()),
                        makeNumber("2")
                    )
                )
            )
        );
    }

    if (name == "atan") {
        // (atan u)' = u' / (1 + u^2)
        return makeBinary(
            BinaryOpType::Divide,
            std::move(du),
            makeBinary(
                BinaryOpType::Add,
                makeNumber("1"),
                makeBinary(
                    BinaryOpType::Power,
                    clone(*call.argument()),
                    makeNumber("2")
                )
            )
        );
    }

    if (name == "exp") {
        // (exp u)' = exp(u) * u'
        return makeBinary(
            BinaryOpType::Multiply,
            makeFunction("exp", clone(*call.argument())),
            std::move(du)
        );
    }

    if (name == "log") {
        // (log u)' = u' / u
        return makeBinary(
            BinaryOpType::Divide,
            std::move(du),
            clone(*call.argument())
        );
    }

    if (name == "sqrt") {
        // (sqrt u)' = u' / (2 * sqrt(u))
        return makeBinary(
            BinaryOpType::Divide,
            std::move(du),
            makeBinary(
                BinaryOpType::Multiply,
                makeNumber("2"),
                makeFunction("sqrt", clone(*call.argument()))
            )
        );
    }

    throw DiffError("Unknown function for differentiation: " + name);
}

NodePtr Differentiator::clone(const Node& node) const {
    if (const auto* number = dynamic_cast<const NumberNode*>(&node)) {
        return makeNumber(number->value());
    }

    if (const auto* variable = dynamic_cast<const VariableNode*>(&node)) {
        return makeVariable(variable->name());
    }

    if (const auto* unary = dynamic_cast<const UnaryOpNode*>(&node)) {
        return makeUnary(
            unary->op(),
            clone(*unary->operand())
        );
    }

    if (const auto* binary = dynamic_cast<const BinaryOpNode*>(&node)) {
        return makeBinary(
            binary->op(),
            clone(*binary->left()),
            clone(*binary->right())
        );
    }

    if (const auto* call = dynamic_cast<const FunctionCallNode*>(&node)) {
        return makeFunction(
            call->function_name(),
            clone(*call->argument())
        );
    }

    throw DiffError("Unknown AST node in clone");
}

NodePtr Differentiator::makeNumber(const std::string& value) const {
    return std::make_unique<NumberNode>(value);
}

NodePtr Differentiator::makeVariable(const std::string& name) const {
    return std::make_unique<VariableNode>(name);
}

NodePtr Differentiator::makeUnary(UnaryOpType op, NodePtr operand) const {
    return std::make_unique<UnaryOpNode>(op, std::move(operand));
}

NodePtr Differentiator::makeBinary(BinaryOpType op, NodePtr left, NodePtr right) const {
    return std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
}

NodePtr Differentiator::makeFunction(const std::string& name, NodePtr argument) const {
    return std::make_unique<FunctionCallNode>(name, std::move(argument));
}