#include "../Statement.hpp"
#include "../../Expressions/Expr.hpp"

class LoxClass;

class ClassStatement : public Statement
{
public:
    Token name;
    std::vector<std::unique_ptr<Statement>> methods;
    std::unique_ptr<Expr> superclass;

    ClassStatement(Token name, std::vector<std::unique_ptr<Statement>> methods, std::unique_ptr<Expr> superclass) : name(std::move(name)), methods(std::move(methods)), superclass(std::move(superclass)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitClassStatement(*this);
    }
};