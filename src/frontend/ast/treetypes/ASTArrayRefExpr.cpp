#include "ASTArrayRefExpr.h"
#include "ASTVisitor.h"
#include "ASTinternal.h"

void ASTArrayRefExpr::accept(ASTVisitor *visitor)
{
    if (visitor->visit(this))
    {
        getArray()->accept(visitor);
        getIndex()->accept(visitor);
    }
    visitor->endVisit(this);
}

std::ostream &ASTArrayRefExpr::print(std::ostream &out) const
{
    out << *getArray() << "[" << *getIndex() << "]";
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTArrayRefExpr::getChildren()
{
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(ARRAY);
    children.push_back(INDEX);
    return children;
}
