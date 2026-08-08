#include "../Statement.hpp"
#include "../../Expressions/Expr.hpp"

class WhileStatement : public Statement
{
public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Statement> body;

    WhileStatement(std::shared_ptr<Expr> condition, std::shared_ptr<Statement> body) : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitWhileStatement(*this);
    }
};