#pragma once

#include "ASTNumberExpr.h"

/*! \brief Class for numeric literals.
 */
class ASTBooleanExpr : public ASTExpr
{
    bool VAL;
    std::shared_ptr<ASTNumberExpr> INT_VAL;

public:
    ASTBooleanExpr(int VAL);
    int getValue() const { return VAL; }
    bool getBoolValue() const { return VAL; }
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};
