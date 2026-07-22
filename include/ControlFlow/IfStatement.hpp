#include "../Statement.hpp"
#include "../Expr.hpp"

class IfStatement : public Statement
{
public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Statement> thenBranch;
    std::shared_ptr<Statement> elseBranch;

    IfStatement(std::shared_ptr<Expr> condition, std::shared_ptr<Statement> thenBranch, std::shared_ptr<Statement> elseBranch) : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitIfStatement(*this);
    }
};