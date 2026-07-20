#include "../Statement.hpp"

class VarStatement : public Statement
{
public:
    const std::string name;
    std::unique_ptr<Expr> initializer;

    VarStatement(std::string name, std::unique_ptr<Expr> initializer) : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(VarVisitor &visitor) override
    {
        visitor.visitVarStatement(*this);
    }
};