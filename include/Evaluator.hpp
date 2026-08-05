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
    void resolve(Expr &expr, int depth);
    std::shared_ptr<Environment> getEnvironment();
    void setEnvironment(std::shared_ptr<Environment> env);

    virtual Value evaluate(Expr &expr);

    Value visitBinaryExpr(Binary &expr) override;
    Value visitGroupingExpr(Grouping &expr) override;
    Value visitLiteralExpr(Literal &expr) override;
    Value visitUnaryExpr(Unary &expr) override;
    Value visitVariableExpr(Variable &expr) override;
    Value visitAssignExpr(Assign &expr) override;
    Value visitCallExpr(Call &expr) override;

    void visitVarStatement(VarStatement &stmt) override;
    void visitPrintStatement(PrintStatement &stmt) override;
    void visitExpressionStatement(ExpressionStatement &stmt) override;
    void visitIfStatement(IfStatement &stmt) override;
    void visitWhileStatement(WhileStatement &stmt) override;
    void visitForStatement(ForStatement &stmt) override;
    void visitBlockStatement(BlockStatement &stmt) override;
    void visitFuncStatement(FuncStatement &stmt) override;
    void visitReturnStatement(ReturnStatement &stmt) override;
    void visitClassStatement(ClassStatement &stmt) override;

private:
    std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    std::shared_ptr<Environment> environment = globals;
    std::unordered_map<Expr *, int> locals;

    void checkNumberOperands(const Value &left, const Value &right);
};