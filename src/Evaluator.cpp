#include "../include/Statements/StatementImports.hpp"
#include "Helper/Functions.hpp"
#include "../include/Evaluator.hpp"
#include "../include/LoxStuff/LoxImports.hpp"

void Evaluator::checkNumberOperands(const Value &left, const Value &right)
{
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
        return;
    throw std::runtime_error("Operands must be numbers.");
}
void Evaluator::resolve(Expr &expr, int depth)
{
    locals[&expr] = depth;
}

std::shared_ptr<Environment> Evaluator::getEnvironment()
{
    return environment;
}

void Evaluator::setEnvironment(std::shared_ptr<Environment> env)
{
    environment = env;
}

Value Evaluator::evaluate(Expr &expr)
{
    return expr.evaluate(*this);
}

Value Evaluator::visitBinaryExpr(Binary &expr)
{
    Value left = evaluate(*expr.left);
    Value right = evaluate(*expr.right);

    switch (expr.op.type)
    {
    case TokenType::PLUS:
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            return std::get<double>(left) + std::get<double>(right);

        if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
            return std::get<std::string>(left) + std::get<std::string>(right);

        throw std::runtime_error("Operands must be two numbers or two strings.");
    case TokenType::MINUS:
        checkNumberOperands(left, right);
        return std::get<double>(left) - std::get<double>(right);
    case TokenType::STAR:
        checkNumberOperands(left, right);
        return std::get<double>(left) * std::get<double>(right);
    case TokenType::SLASH:
        checkNumberOperands(left, right);
        return std::get<double>(left) / std::get<double>(right);

    case TokenType::EQUAL_EQUAL:
        return left == right;
    case TokenType::NOT_EQUAL:
        return left != right;
    case TokenType::GREATER:
        checkNumberOperands(left, right);
        return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(left, right);
        return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESS:
        checkNumberOperands(left, right);
        return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESS_EQUAL:
        checkNumberOperands(left, right);
        return std::get<double>(left) <= std::get<double>(right);
    default:
        throw std::runtime_error("Unknown binary operator.");
    }
    return nullptr;
}
Value Evaluator::visitGroupingExpr(Grouping &expr)
{
    return evaluate(*expr.expression);
}
Value Evaluator::visitLiteralExpr(Literal &expr)
{
    return expr.value;
}
Value Evaluator::visitUnaryExpr(Unary &expr)
{
    Value right = evaluate(*expr.right);

    switch (expr.op.type)
    {
    case TokenType::NOT:
        if (std::holds_alternative<bool>(right))
            return !std::get<bool>(right);

        throw std::runtime_error("Operand must be bool");
    case TokenType::MINUS:
        if (std::holds_alternative<double>(right))
            return std::get<double>(right) * -1;

        throw std::runtime_error("Operand must be number");
    default:
        throw std::runtime_error("Unknown unary operator.");
    }

    return nullptr;
}

Value Evaluator::visitVariableExpr(Variable &expr)
{
    auto it = locals.find(&expr);
    if (it != locals.end())
    {
        return environment->getAt(it->second, expr.name.lexeme);
    }
    return globals->get(expr.name.lexeme);
}
Value Evaluator::visitAssignExpr(Assign &expr)
{
    Value value = evaluate(*expr.value);
    auto it = locals.find(&expr);
    if (it != locals.end())
    {
        environment->assignAt(it->second, expr.name.lexeme, value);
    }

    globals->assign(expr.name.lexeme, value);

    return value;
}
Value Evaluator::visitCallExpr(Call &expr)
{
    Value calleeValue = evaluate(*expr.callee);

    if (!std::holds_alternative<std::shared_ptr<Callable>>(calleeValue))
    {
        throw std::runtime_error("Can only call functions.");
    }
    auto function = std::get<std::shared_ptr<Callable>>(calleeValue);

    std::vector<Value> arguments;
    for (const auto &arg : expr.arguments)
    {
        arguments.push_back(evaluate(*arg));
    }

    if (arguments.size() != function->arity())
    {
        throw std::runtime_error("Expected " + std::to_string(function->arity()) +
                                 " arguments but got " + std::to_string(arguments.size()) + ".");
    }

    return function->call(*this, arguments);
}
Value Evaluator::visitGetExpr(Get &expr)
{
    Value object = evaluate(*expr.object);

    if (!std::holds_alternative<std::shared_ptr<LoxInstance>>(object))
    {
        throw std::runtime_error("Only instances have properties.");
    }

    return std::get<std::shared_ptr<LoxInstance>>(object)->get(expr.name.lexeme);
}
Value Evaluator::visitSetExpr(Set &expr)
{
    Value object = evaluate(*expr.object);

    if (!std::holds_alternative<std::shared_ptr<LoxInstance>>(object))
    {
        throw std::runtime_error("Only instances have fields.");
    }

    Value value = evaluate(*expr.value);
    std::get<std::shared_ptr<LoxInstance>>(object)->set(expr.name.lexeme, value);
    return value;
}
Value Evaluator::visitSuperExpr(Super &expr)
{
    auto it = locals.find(&expr);
    if (it == locals.end())
    {
        throw std::runtime_error("Internal error: 'super' not resolved.");
    }

    int distance = it->second;

    auto superclassValue = environment->getAt(distance, "super");
    auto superclassCallable = std::get<std::shared_ptr<Callable>>(superclassValue);
    auto superclass = std::dynamic_pointer_cast<LoxClass>(superclassCallable);
    if (superclass == nullptr)
    {

        throw std::runtime_error("Internal error: 'super' did not resolve to a class.");
    }

    auto instanceValue = environment->getAt(distance - 1, "this");
    auto instance = std::get<std::shared_ptr<LoxInstance>>(instanceValue);

    auto method = superclass->findMethod(expr.method.lexeme);
    if (method == nullptr)
    {
        throw std::runtime_error("Undefined property '" + expr.method.lexeme + "'.");
    }

    return method->bind(instance);
}
void Evaluator::visitVarStatement(VarStatement &stmt)
{
    Value val = nullptr;
    if (stmt.initializer)
    {
        val = evaluate(*stmt.initializer);
    }

    environment->define(stmt.name.lexeme, val);
}
void Evaluator::visitPrintStatement(PrintStatement &stmt)
{
    Value val = evaluate(*stmt.expression);
    std::cout << stringify(val) << std::endl;
}

void Evaluator::visitExpressionStatement(ExpressionStatement &stmt)
{
    evaluate(*stmt.expression);
}
void Evaluator::visitIfStatement(IfStatement &stmt)
{
    Value conditionResult = evaluate(*stmt.condition);

    if (isTruthy(conditionResult))
    {
        stmt.thenBranch->accept(*this);
    }
    else if (stmt.elseBranch != nullptr)
    {
        stmt.elseBranch->accept(*this);
    }
}
void Evaluator::visitWhileStatement(WhileStatement &stmt)
{

    while (isTruthy(evaluate(*stmt.condition)))
    {
        stmt.body->accept(*this);
    }
}
void Evaluator::visitForStatement(ForStatement &stmt)
{
    if (stmt.initializer != nullptr)
    {
        stmt.initializer->accept(*this);
    }

    while (stmt.condition == nullptr || isTruthy(evaluate(*stmt.condition)))
    {
        stmt.body->accept(*this);

        if (stmt.increment != nullptr)
        {
            evaluate(*stmt.increment);
        }
    }
}

void Evaluator::visitBlockStatement(BlockStatement &stmt)
{
    auto blockEnv = std::make_shared<Environment>(environment);

    auto previousEnv = std::move(environment);
    environment = std::move(blockEnv);

    for (const auto &statement : stmt.statements)
    {
        statement->accept(*this);
    }

    environment = std::move(previousEnv);
}

void Evaluator::visitFuncStatement(FuncStatement &stmt)
{
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(&stmt, environment);
    environment->define(stmt.name.lexeme, function);
}

void Evaluator::visitReturnStatement(ReturnStatement &stmt)
{
    Value value = nullptr;
    if (stmt.value != nullptr)
        value = evaluate(*stmt.value);

    throw ReturnException(value, "return statement");
}

void Evaluator::visitClassStatement(ClassStatement &stmt)
{
    std::shared_ptr<LoxClass> superclass = nullptr;
    if (stmt.superclass != nullptr)
    {
        Value superclassValue = evaluate(*stmt.superclass);
        auto callee = std::get<std::shared_ptr<Callable>>(superclassValue);
        superclass = std::dynamic_pointer_cast<LoxClass>(callee);
        if (superclass == nullptr)
        {
            throw std::runtime_error("Superclass must be a class.");
        }
    }

    environment->define(stmt.name.lexeme, nullptr);

    if (stmt.superclass != nullptr)
    {
        environment = std::make_shared<Environment>(environment);
        environment->define("super", superclass);
    }

    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
    for (const auto &methodStmt : stmt.methods)
    {
        auto *funcStmt = dynamic_cast<FuncStatement *>(methodStmt.get());
        auto function = std::make_shared<LoxFunction>(funcStmt, environment);
        methods[funcStmt->name.lexeme] = function;
    }

    auto klass = std::make_shared<LoxClass>(methods, stmt.name.lexeme, superclass);

    if (stmt.superclass != nullptr)
    {
        environment = environment->enclosing;
    }

    environment->define(stmt.name.lexeme, klass);
}