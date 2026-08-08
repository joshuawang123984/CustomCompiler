#include "../Statement.hpp"
#include "../../Expressions/Expr.hpp"
#include "../StatementVisitor.hpp"

class FuncStatement : public Statement
{
public:
    Token name;
    std::vector<Token> arguments;
    std::unique_ptr<Statement> body;

    FuncStatement(Token name, std::vector<Token> arguments, std::unique_ptr<Statement> body) : name(std::move(name)), arguments(std::move(arguments)), body(std::move(body)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitFuncStatement(*this);
    }
};