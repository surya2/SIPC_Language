#pragma once

#include "ASTStmt.h"
#include "ASTExpr.h"

class ASTTernaryExpr : public ASTExpr
{
    std::shared_ptr<ASTExpr> COND;
    std::shared_ptr<ASTExpr> TRUEEXPR;
    std::shared_ptr<ASTExpr> FALSEEXPR;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTTernaryExpr(std::shared_ptr<ASTExpr> COND, std::shared_ptr<ASTExpr> TRUEEXPR, std::shared_ptr<ASTExpr> FALSEEXPR)
        : COND(COND), TRUEEXPR(TRUEEXPR), FALSEEXPR(FALSEEXPR) {}
    ASTExpr *getCondition() const { return COND.get(); }
    ASTExpr *getTrueExpr() const { return TRUEEXPR.get(); }
    ASTExpr *getFalseExpr() const { return FALSEEXPR.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};