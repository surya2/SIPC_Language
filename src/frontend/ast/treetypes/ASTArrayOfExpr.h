#pragma once

#include "ASTDeclNode.h"
#include "ASTExpr.h"

/*! \brief Class for defining a record.
 */
class ASTArrayOfExpr : public ASTExpr
{
    std::shared_ptr<ASTExpr> LEN_EXPR;
    std::shared_ptr<ASTExpr> ELEMENT_EXPR;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTArrayOfExpr(std::shared_ptr<ASTExpr> LEN_EXPR, std::shared_ptr<ASTExpr> ELEMENT_EXPR)
        : LEN_EXPR(LEN_EXPR), ELEMENT_EXPR(ELEMENT_EXPR) {};
    ASTExpr *getLength() const { return LEN_EXPR.get(); }
    ASTExpr *getElement() const { return ELEMENT_EXPR.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};