#pragma once

#include "ASTDeclNode.h"
#include "ASTExpr.h"

/*! \brief Class for defining a record.
 */
class ASTArrayRefExpr : public ASTExpr
{
    std::shared_ptr<ASTExpr> ARRAY;
    std::shared_ptr<ASTExpr> INDEX;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTArrayRefExpr(std::shared_ptr<ASTExpr> ARRAY, std::shared_ptr<ASTExpr> INDEX) : ARRAY(ARRAY), INDEX(INDEX) {};
    ASTExpr *getArray() const { return ARRAY.get(); }
    ASTExpr *getIndex() const { return INDEX.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};