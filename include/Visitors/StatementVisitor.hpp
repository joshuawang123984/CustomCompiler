#pragma once

class VarStatement;
class PrintStatement;
class ExpressionStatement;
class IfStatement;
class WhileStatement;
class ForStatement;

class StatementVisitor
{
public:
    virtual ~StatementVisitor() = default;
    virtual void visitVarStatement(VarStatement &stmt) = 0;
    virtual void visitPrintStatement(PrintStatement &stmt) = 0;
    virtual void visitExpressionStatement(ExpressionStatement &stmt) = 0;
    virtual void visitIfStatement(IfStatement &stmt) = 0;
    virtual void visitWhileStatement(WhileStatement &stmt) = 0;
    virtual void visitForStatement(ForStatement &stmt) = 0;
};