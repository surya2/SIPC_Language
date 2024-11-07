#pragma once

#include "ASTDeclNode.h"
#include "ASTExpr.h"

/*! \brief Class for defining a record.
 */
class ASTArrayExpr : public ASTExpr
{
public:
    std::vector<std::shared_ptr<ASTExpr>> ITEMS;
    int LEN;
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTArrayExpr(std::vector<std::shared_ptr<ASTExpr>> EXPRS, int LEN);
    std::vector<ASTExpr *> getItems() const;
    int getLen() const { return LEN; };
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};