#pragma once

class VarStatement;
class PrintStatement;
class ExpressionStatement;

class StatementVisitor
{
public:
    virtual ~StatementVisitor() = default;
    virtual void visitVarStatement(VarStatement &stmt) = 0;
    virtual void visitPrintStatement(PrintStatement &stmt) = 0;
    virtual void visitExpressionStatement(ExpressionStatement &stmt) = 0;
};