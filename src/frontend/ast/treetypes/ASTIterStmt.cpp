#include "ASTIterStmt.h"
#include "ASTVisitor.h"
#include "ASTinternal.h"

void ASTIterStmt::accept(ASTVisitor *visitor)
{
    if (visitor->visit(this))
    {
        getElement()->accept(visitor);
        getIterable()->accept(visitor);
        getBody()->accept(visitor);
    }
    visitor->endVisit(this);
}

// Do I need to print it like the for loop is written by the developer with the ':', '..' and 'by'?
std::ostream &ASTIterStmt::print(std::ostream &out) const
{
    out << "for (" << *getElement() << " : " << *getIterable() << ") " << *getBody();
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTIterStmt::getChildren()
{
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(this->ELEMENT);
    children.push_back(this->ITERABLE);
    children.push_back(BODY);
    return children;
}