#include "ASTForLoopStmt.h"
#include "ASTVisitor.h"
#include "ASTinternal.h"

ASTForLoopStmt::ASTForLoopStmt(std::vector<std::shared_ptr<ASTExpr>> EXPRS, std::shared_ptr<ASTStmt> BODY)
{
    this->BODY = BODY;
    for (auto &expr : EXPRS)
    {
        std::shared_ptr<ASTExpr> e = expr;
        this->EXPRS.push_back(e);
    }
    if (this->EXPRS.size() == 3)
    {
        this->VAR = this->EXPRS[0];
        this->START = this->EXPRS[1];
        this->END = this->EXPRS[2];
        this->STEP = nullptr;
    }
    else if (this->EXPRS.size() == 4)
    {
        this->VAR = this->EXPRS[0];
        this->START = this->EXPRS[1];
        this->END = this->EXPRS[2];
        this->STEP = this->EXPRS[3];
    }
}

std::vector<ASTExpr *> ASTForLoopStmt::getExprs() const { return rawRefs(EXPRS); }

void ASTForLoopStmt::accept(ASTVisitor *visitor)
{
    if (visitor->visit(this))
    {
        for (auto e : getExprs())
        {
            e->accept(visitor);
        }
        getBody()->accept(visitor);
    }
    visitor->endVisit(this);
}

// Do I need to print it like the for loop is written by the developer with the ':', '..' and 'by'?
std::ostream &ASTForLoopStmt::print(std::ostream &out) const
{
    out << "for (" << *VAR << ":" << *START << " .. " << *END;
    if (STEP)
    {
        out << " by " << *STEP;
    }
    out << ") " << *getBody();

    return out;
}
// LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTForLoopStmt::getChildren()
{
    std::vector<std::shared_ptr<ASTNode>> children;
    for (auto &expr : EXPRS)
    {
        children.push_back(expr);
    }
    children.push_back(BODY);
    return children;
}