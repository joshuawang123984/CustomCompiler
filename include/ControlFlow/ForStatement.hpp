#include "../Statement.hpp"
#include "../Expr.hpp"

class ForStatement : public Statement
{
public:
    std::shared_ptr<Statement> initializer;
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> increment;
    std::shared_ptr<Statement> body;

    ForStatement(std::shared_ptr<Statement> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> increment, std::shared_ptr<Statement> body) : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitForStatement(*this);
    }
};