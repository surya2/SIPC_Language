#pragma once

#include "ASTStmt.h"
#include "ASTExpr.h"

class ASTIterStmt : public ASTStmt
{
    // std::vector<std::shared_ptr<ASTExpr>> EXPRS;
    std::shared_ptr<ASTStmt> BODY;
    std::shared_ptr<ASTExpr> ELEMENT;
    std::shared_ptr<ASTExpr> ITERABLE;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTIterStmt(std::shared_ptr<ASTExpr> EXPR1, std::shared_ptr<ASTExpr> EXPR2, std::shared_ptr<ASTStmt> BODY)
        : ELEMENT(EXPR1), ITERABLE(EXPR2), BODY(BODY) {}
    void accept(ASTVisitor *visitor) override;
    ASTStmt *getBody() const { return BODY.get(); }
    ASTExpr *getElement() const { return ELEMENT.get(); }
    ASTExpr *getIterable() const { return ITERABLE.get(); }
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};