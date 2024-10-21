#pragma once

#include "ASTStmt.h"
#include "ASTExpr.h"

class ASTForLoopStmt : public ASTStmt
{
    std::vector<std::shared_ptr<ASTExpr>> EXPRS;
    std::shared_ptr<ASTStmt> BODY;
    std::shared_ptr<ASTExpr> VAR;
    std::shared_ptr<ASTExpr> START;
    std::shared_ptr<ASTExpr> END;
    std::shared_ptr<ASTExpr> STEP;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTForLoopStmt(std::vector<std::shared_ptr<ASTExpr>> EXPRS, std::shared_ptr<ASTStmt> BODY);
    std::vector<ASTExpr *> getExprs() const;
    void accept(ASTVisitor *visitor) override;
    ASTStmt *getBody() const { return BODY.get(); }
    ASTExpr *getVar() const { return VAR.get(); }
    ASTExpr *getStart() const { return START.get(); }
    ASTExpr *getEnd() const { return END.get(); }
    ASTExpr *getStep() const { return STEP.get(); }
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};