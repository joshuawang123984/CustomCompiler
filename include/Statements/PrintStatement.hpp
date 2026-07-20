#include "../Statement.hpp"

class PrintStatement : public Statement
{
public:
    std::unique_ptr<Expr> expression;

    PrintStatement(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitPrintStatement(*this);
    }
};