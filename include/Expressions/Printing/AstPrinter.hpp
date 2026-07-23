#include "AstVisitor.hpp"
#include "Helper/Functions.hpp"
#include "../Expr.hpp"

class AstPrinter : public AstVisitor
{
public:
    std::string print(Expr &expr)
    {
        return expr.accept(*this);
    }
    std::string visitBinary(Binary &expr) override
    {
        return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
    }
    std::string visitGrouping(Grouping &expr) override
    {
        return parenthesize("grouping", {expr.expression.get()});
    }
    std::string visitLiteral(Literal &expr) override
    {
        return stringify(expr.value);
    }
    std::string visitUnary(Unary &expr) override
    {
        return parenthesize(expr.op.lexeme, {expr.right.get()});
    }
    std::string visitVariable(Variable &expr) override
    {
        std::string result = "Variable Name: " + expr.name;

        return result;
    }

private:
    std::string parenthesize(const std::string &name, std::vector<Expr *> exprs)
    {
        std::string result = "(" + name;
        for (Expr *expr : exprs)
        {
            result += " " + expr->accept(*this);
        }
        result += ")";
        return result;
    }
};