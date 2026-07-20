#pragma once

class VarStatement;
class PrintStatement;

class StatementVisitor
{
public:
    virtual ~StatementVisitor() = default;
    virtual void visitVarStatement(VarStatement &stmt) = 0;
    virtual void visitPrintStatement(PrintStatement &stmt) = 0;
};