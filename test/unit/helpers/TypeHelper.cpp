#include "TypeHelper.h"

#include "TipAlpha.h"
#include "TipFunction.h"
#include "TipInt.h"
#include "TipBool.h"
#include "TipMu.h"
#include "TipRef.h"
#include "TipRecord.h"
#include "TipAbsentField.h"
#include "SipArray.h"

std::shared_ptr<TipType> TypeHelper::intType()
{
    return std::make_shared<TipInt>();
}

std::shared_ptr<TipType> TypeHelper::boolType()
{
    return std::make_shared<TipBool>();
}

std::shared_ptr<TipType> TypeHelper::alphaType(ASTNode *node)
{
    return std::make_shared<TipAlpha>(node);
}

std::shared_ptr<TipType> TypeHelper::ptrType(std::shared_ptr<TipType> t)
{
    return std::make_shared<TipRef>(t);
}

std::shared_ptr<TipType> TypeHelper::funType(std::vector<std::shared_ptr<TipType>> p, std::shared_ptr<TipType> r)
{
    return std::make_shared<TipFunction>(p, r);
}

std::shared_ptr<TipType> TypeHelper::recType(std::vector<std::shared_ptr<TipType>> t, std::vector<std::string> n)
{
    return std::make_shared<TipRecord>(t, n);
}

std::shared_ptr<TipType> TypeHelper::absentType()
{
    return std::make_shared<TipAbsentField>();
}

std::shared_ptr<TipType> TypeHelper::arrayType(std::shared_ptr<TipType> t, std::vector<std::shared_ptr<TipType>> ts)
{
    return std::make_shared<SipArray>(ts);
}
