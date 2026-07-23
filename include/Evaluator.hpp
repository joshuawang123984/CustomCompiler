#pragma once
#include "Statements/StatementVisitor.hpp"
#include "Statements/Statement.hpp"
#include "Expressions/EvaluatorVisitor.hpp"
#include "Environment.hpp"
#include "Expressions/Expr.hpp"

class Evaluator : public EvaluatorVisitor, public StatementVisitor
{
public:
    virtual ~Evaluator() = default;
    virtual Value evaluate(Expr &expr);

    Value visitBinaryExpr(Binary &expr) override;
    Value visitGroupingExpr(Grouping &expr) override;
    Value visitLiteralExpr(Literal &expr) override;
    Value visitUnaryExpr(Unary &expr) override;
    Value visitVariableExpr(Variable &expr) override;

    void visitVarStatement(VarStatement &stmt) override;
    void visitPrintStatement(PrintStatement &stmt) override;
    void visitExpressionStatement(ExpressionStatement &stmt) override;
    void visitIfStatement(IfStatement &stmt) override;
    void visitWhileStatement(WhileStatement &stmt) override;
    void visitForStatement(ForStatement &stmt) override;
    void visitBlockStatement(BlockStatement &stmt) override;

private:
    std::unique_ptr<Environment> environment = std::make_unique<Environment>();

    void checkNumberOperands(const Value &left, const Value &right);
};