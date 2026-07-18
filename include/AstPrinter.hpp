#include "AstVisitor.hpp"
#include "Expr.hpp"

class AstPrinter : public AstVisitor
{
public:
    std::string print(Expr &expr)
    {
        return expr.accept(*this);
    }
    std::string visitBinary(Binary &expr) {}
    std::string visitGrouping(Grouping &expr) {}
    std::string visitLiteral(Literal &expr) {}
    std::string visitUnary(Unary &expr) {}

private:
};