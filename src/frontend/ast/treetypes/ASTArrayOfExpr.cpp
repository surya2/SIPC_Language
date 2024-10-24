#include "ASTArrayOfExpr.h"
#include "ASTVisitor.h"
#include "ASTinternal.h"

void ASTArrayOfExpr::accept(ASTVisitor *visitor)
{
    if (visitor->visit(this))
    {
        getLength()->accept(visitor);
        getElement()->accept(visitor);
    }
    visitor->endVisit(this);
}

std::ostream &ASTArrayOfExpr::print(std::ostream &out) const
{
    out << "[";
    out << *getLength() << " of " << *getElement();
    out << "]";
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTArrayOfExpr::getChildren()
{
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(LEN_EXPR);
    children.push_back(ELEMENT_EXPR);
    return children;
}
