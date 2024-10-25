#include "ASTTernaryExpr.h"
#include "ASTVisitor.h"
#include "ASTinternal.h"

std::ostream &ASTTernaryExpr::print(std::ostream &out) const
{
    out << *getCondition() << " ? " << *getTrueExpr() << " : " << *getFalseExpr();
    return out;
} // LCOV_EXCL_LINE

void ASTTernaryExpr::accept(ASTVisitor *visitor)
{
    if (visitor->visit(this))
    {
        getCondition()->accept(visitor);
        getTrueExpr()->accept(visitor);
        getFalseExpr()->accept(visitor);
    }
    visitor->endVisit(this);
}

std::vector<std::shared_ptr<ASTNode>> ASTTernaryExpr::getChildren()
{
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(COND);
    children.push_back(TRUEEXPR);
    children.push_back(FALSEEXPR);
    return children;
}