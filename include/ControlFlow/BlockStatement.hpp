#include "../Statement.hpp"
#include "../Expr.hpp"

class BlockStatement : public Statement
{
public:
    std::vector<std::unique_ptr<Statement>> statements;
    BlockStatement(std::vector<std::unique_ptr<Statement>> statements) : statements(std::move(statements)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitBlockStatement(*this);
    }
};