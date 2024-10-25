#include "ASTIncDecStmt.h"
#include "ASTVisitor.h"

void ASTIncDecStmt::accept(ASTVisitor *visitor)
{
  if (visitor->visit(this))
  {
    getExpr()->accept(visitor);
  }
  visitor->endVisit(this);
}

std::ostream &ASTIncDecStmt::print(std::ostream &out) const
{
  out << *getExpr() << getOp() << ";";
  return out;
}

std::vector<std::shared_ptr<ASTNode>> ASTIncDecStmt::getChildren()
{
  std::vector<std::shared_ptr<ASTNode>> children;
  children.push_back(EXPR);
  return children;
}
