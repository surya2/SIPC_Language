#include "ASTBooleanExpr.h"
#include "ASTVisitor.h"

#include <iostream>

ASTBooleanExpr::ASTBooleanExpr(int value)
{
    VAL = value;
    int int_value = value;
    INT_VAL = std::make_shared<ASTNumberExpr>(int_value);
}

void ASTBooleanExpr::accept(ASTVisitor *visitor)
{
    visitor->visit(this);
    visitor->endVisit(this);
}

std::ostream &ASTBooleanExpr::print(std::ostream &out) const
{
    out << getBoolValue();
    return out;
}
