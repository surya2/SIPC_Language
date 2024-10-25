#pragma once

#include "ASTExpr.h"
#include "ASTStmt.h"

/*! \brief Class for assignment
 */
class ASTIncDecStmt : public ASTStmt
{
  std::string OP;
  std::shared_ptr<ASTExpr> EXPR;

public:
  std::vector<std::shared_ptr<ASTNode>> getChildren() override;
  ASTIncDecStmt(const std::string &OP, std::shared_ptr<ASTExpr> EXPR)
      : OP(OP), EXPR(EXPR) {}
  ASTExpr *getExpr() const { return EXPR.get(); }
  std::string getOp() const { return OP; }
  void accept(ASTVisitor *visitor) override;
  llvm::Value *codegen() override;

protected:
  std::ostream &print(std::ostream &out) const override;
};
