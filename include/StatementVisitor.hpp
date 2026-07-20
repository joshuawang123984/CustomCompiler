#include "Statements/VarStatement.hpp"
#include "Statements/PrintStatement.hpp"

class StatementVisitor
{
public:
    virtual ~StatementVisitor() = default;
    virtual void visitVarStatement(VarStatement &stmt) = 0;
    virtual void PrintVarStatement(PrintStatement &stmt) = 0;
};