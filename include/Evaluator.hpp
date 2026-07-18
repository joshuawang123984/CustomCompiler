#include "EvaluatorVisitor.hpp"
#include "Expr.hpp"

class Evaluator : public EvaluatorVisitor
{
    virtual ~Evaluator() = default;
    virtual Value evaluate(Expr &expr);

    Value visitBinaryExpr(Binary &expr) override;
    Value visitGroupingExpr(Grouping &expr) override;
    Value visitLiteralExpr(Literal &expr) override;
    Value visitUnaryExpr(Unary &expr) override;
};