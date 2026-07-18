#include "AstVisitor.hpp"
#include "Expr.hpp"

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

private:
    std::string stringify(Value value)
    {
        if (std::holds_alternative<double>(value))
            return std::to_string(std::get<double>(value));

        if (std::holds_alternative<bool>(value))
            return std::get<bool>(value) ? "true" : "false";

        if (std::holds_alternative<std::string>(value))
            return std::get<std::string>(value);

        if (std::holds_alternative<std::nullptr_t>(value))
            return "nil";

        return "unknown";
    }

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