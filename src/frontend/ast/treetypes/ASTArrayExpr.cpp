#include "ASTArrayExpr.h"
#include "ASTVisitor.h"
#include "ASTinternal.h"

ASTArrayExpr::ASTArrayExpr(std::vector<std::shared_ptr<ASTExpr>> EXPRS, int LEN)
{
    for (auto &expr : EXPRS)
    {
        std::shared_ptr<ASTExpr> e = expr;
        this->ITEMS.push_back(e);
    }
    this->LEN = LEN;
}

std::vector<ASTExpr *> ASTArrayExpr::getItems() const
{
    return rawRefs(ITEMS);
}

void ASTArrayExpr::accept(ASTVisitor *visitor)
{
    if (visitor->visit(this))
    {
        for (auto expr : getItems())
        {
            expr->accept(visitor);
        }
    }
    visitor->endVisit(this);
}

std::ostream &ASTArrayExpr::print(std::ostream &out) const
{
    out << "[";
    bool first = true;
    for (auto &expr : getItems())
    {
        if (first)
        {
            first = false;
            out << *expr;
            continue;
        }
        out << "," << *expr;
    }
    out << "]";
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTArrayExpr::getChildren()
{
    std::vector<std::shared_ptr<ASTNode>> children;
    for (auto &expr : ITEMS)
    {
        children.push_back(expr);
    }
    return children;
}
