#include "Expressions/EvaluatorVisitor.hpp"
#include "Statements/StatementVisitor.hpp"
#include "Evaluator.hpp"
#include "Statements/StatementImports.hpp"

class Resolver : public EvaluatorVisitor, public StatementVisitor
{
private:
    Evaluator &evaluator;
    std::vector<std::unordered_map<std::string, bool>> scopes;

public:
    Resolver(Evaluator &evaluator) : evaluator(evaluator) {}

    void beginScope()
    {
        scopes.push_back({});
    }

    void endScope()
    {
        scopes.pop_back();
    }

    void declare(const Token &name)
    {
        if (scopes.empty())
            return;
        scopes.back()[name.lexeme] = false;
    }

    void define(const Token &name)
    {
        if (scopes.empty())
            return;
        scopes.back()[name.lexeme] = true;
    }

    void resolveLocal(Expr &expr, const Token &name)
    {
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            if (scopes[i].find(name.lexeme) != scopes[i].end())
            {
                evaluator.resolve(expr, scopes.size() - i - 1);
                return;
            }
        }
    }

    void resolve(const std::vector<std::unique_ptr<Statement>> &statements)
    {
        for (const auto &stmt : statements)
            resolve(*stmt);
    }

    void resolve(Statement &stmt)
    {
        stmt.accept(*this);
    }

    void resolve(Expr &expr)
    {
        expr.evaluate(*this);
    }

    Value visitVariableExpr(Variable &expr) override
    {
        if (!scopes.empty())
        {
            auto &currentScope = scopes.back();
            auto it = currentScope.find(expr.name.lexeme);
            if (it != currentScope.end() && it->second == false)
            {
                throw std::runtime_error("Can't read local variable in its own initializer.");
            }
        }
        resolveLocal(expr, expr.name);
        return nullptr;
    }

    Value visitAssignExpr(Assign &expr) override
    {
        resolve(*expr.value);
        resolveLocal(expr, expr.name);
        return nullptr;
    }

    Value visitBinaryExpr(Binary &expr) override
    {
        resolve(*expr.left);
        resolve(*expr.right);
        return nullptr;
    }

    Value visitGroupingExpr(Grouping &expr) override
    {
        resolve(*expr.expression);
        return nullptr;
    }

    Value visitLiteralExpr(Literal &expr) override
    {
        return nullptr;
    }

    Value visitUnaryExpr(Unary &expr) override
    {
        resolve(*expr.right);
        return nullptr;
    }

    Value visitCallExpr(Call &expr) override
    {
        resolve(*expr.callee);
        for (const auto &arg : expr.arguments)
        {
            resolve(*arg);
        }
        return nullptr;
    }

    Value visitGetExpr(Get &expr) override
    {
        resolve(*expr.object);
        return nullptr;
    }

    Value visitSetExpr(Set &expr) override
    {
        resolve(*expr.object);
        resolve(*expr.value);
        return nullptr;
    }

    void visitBlockStatement(BlockStatement &stmt) override
    {
        beginScope();
        resolve(stmt.statements);
        endScope();
    }

    void visitVarStatement(VarStatement &stmt) override
    {
        declare(stmt.name);
        if (stmt.initializer != nullptr)
            resolve(*stmt.initializer);
        define(stmt.name);
    }
    void visitPrintStatement(PrintStatement &stmt) override
    {
        resolve(*stmt.expression);
    }
    void visitExpressionStatement(ExpressionStatement &stmt) override
    {
        resolve(*stmt.expression);
    }
    void visitIfStatement(IfStatement &stmt) override
    {
        resolve(*stmt.condition);
        resolve(*stmt.thenBranch);
        if (stmt.elseBranch != nullptr)
            resolve(*stmt.elseBranch);
    }
    void visitWhileStatement(WhileStatement &stmt) override
    {
        resolve(*stmt.condition);
        resolve(*stmt.body);
    }
    void visitForStatement(ForStatement &stmt) override
    {
        if (stmt.initializer != nullptr)
            resolve(*stmt.initializer);
        if (stmt.condition != nullptr)
            resolve(*stmt.condition);
        if (stmt.increment != nullptr)
            resolve(*stmt.increment);
        resolve(*stmt.body);
    }
    void visitFuncStatement(FuncStatement &stmt) override
    {
        declare(stmt.name);
        define(stmt.name);

        beginScope();
        for (const auto &param : stmt.arguments)
        {
            declare(param);
            define(param);
        }
        resolve(*stmt.body);
        endScope();
    }
    void visitReturnStatement(ReturnStatement &stmt) override
    {
        if (stmt.value != nullptr)
            resolve(*stmt.value);
    }
    void visitClassStatement(ClassStatement &stmt) override
    {
        declare(stmt.name);
        define(stmt.name);
    }
};