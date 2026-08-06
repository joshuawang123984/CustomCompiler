#include "../Statement.hpp"
#include "../../Expressions/Expr.hpp"

class ClassStatement : public Statement
{
public:
    Token name;
    std::vector<std::unique_ptr<Statement>> methods;

    ClassStatement(Token name, std::vector<std::unique_ptr<Statement>> methods) : name(std::move(name)), methods(std::move(methods)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitClassStatement(*this);
    }
};