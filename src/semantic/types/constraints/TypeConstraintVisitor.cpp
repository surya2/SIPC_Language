#include "TypeConstraintVisitor.h"
#include "TipAbsentField.h"
#include "TipAlpha.h"
#include "TipFunction.h"
#include "TipInt.h"
#include "TipBool.h"
#include "TipRecord.h"
#include "TipRef.h"
#include "TipVar.h"
#include "SipArray.h"

#include <memory>

TypeConstraintVisitor::TypeConstraintVisitor(
    SymbolTable *st, std::shared_ptr<ConstraintHandler> handler)
    : symbolTable(st), constraintHandler(std::move(handler)) {};

/*! \fn astToVar
 *  \brief Convert an AST node to a type variable.
 *
 * Utility function that creates type variables and uses declaration nodes
 * as a canonical representative for program variables.  There are two case
 * that need to be checked: if the variable is local to a function or if
 * it is a function value.
 */
std::shared_ptr<TipType> TypeConstraintVisitor::astToVar(ASTNode *n)
{
  if (auto ve = dynamic_cast<ASTVariableExpr *>(n))
  {
    ASTDeclNode *canonical;
    if ((canonical = symbolTable->getLocal(ve->getName(), scope.top())))
    {
      return std::make_shared<TipVar>(canonical);
    }
    else if ((canonical = symbolTable->getFunction(ve->getName())))
    {
      return std::make_shared<TipVar>(canonical);
    }
  } // LCOV_EXCL_LINE

  return std::make_shared<TipVar>(n);
}

bool TypeConstraintVisitor::visit(ASTFunction *element)
{
  scope.push(element->getDecl());
  return true;
}

/*! \brief Type constraints for function definition.
 *
 * Type rules for "main(X1, ..., Xn) { ... return E; }":
 *   [[X1]] = [[Xn]] = [[E]] = int
 * To express this we will equate all type variables to int.
 *
 * Type rules for "X(X1, ..., Xn) { ... return E; }":
 *   [[X]] = ([[X1]], ..., [[Xn]]) -> [[E]]
 */
void TypeConstraintVisitor::endVisit(ASTFunction *element)
{
  if (element->getName() == "main")
  {
    std::vector<std::shared_ptr<TipType>> formals;
    for (auto &f : element->getFormals())
    {
      formals.push_back(astToVar(f));
      // all formals are int
      constraintHandler->handle(astToVar(f), std::make_shared<TipInt>());
    }

    // Return is the last statement and must be int
    auto ret = dynamic_cast<ASTReturnStmt *>(element->getStmts().back());
    constraintHandler->handle(astToVar(ret->getArg()),
                              std::make_shared<TipInt>());

    constraintHandler->handle(
        astToVar(element->getDecl()),
        std::make_shared<TipFunction>(formals, astToVar(ret->getArg())));
  }
  else
  {
    std::vector<std::shared_ptr<TipType>> formals;
    for (auto &f : element->getFormals())
    {
      formals.push_back(astToVar(f));
    }

    // Return is the last statement
    auto ret = dynamic_cast<ASTReturnStmt *>(element->getStmts().back());

    constraintHandler->handle(
        astToVar(element->getDecl()),
        std::make_shared<TipFunction>(formals, astToVar(ret->getArg())));
  }
}

/*! \brief Type constraints for numeric literal.
 *
 * Type rules for "I":
 *   [[I]] = int
 */
void TypeConstraintVisitor::endVisit(ASTNumberExpr *element)
{
  constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
}

/*! \brief Type constraints for boolean literal.
 *
 * Type rules for "B", a boolean literal like "true" or "false":
 *   [[B]] = bool
 */
void TypeConstraintVisitor::endVisit(ASTBooleanExpr *element)
{
  constraintHandler->handle(astToVar(element), std::make_shared<TipBool>());
}

/*! \brief Type constraints for binary operators.
 *
 * Type rules for "E1 op E2":
 *   [[E1 op E2]] = int
 * and if "op" is not equality or disequality
 *   [[E1]] = [[E2]] = int
 * otherwise
 *   [[E1]] = [[E2]]
 */
void TypeConstraintVisitor::endVisit(ASTBinaryExpr *element)
{
  auto op = element->getOp();
  auto intType = std::make_shared<TipInt>();
  auto boolType = std::make_shared<TipBool>();

  // result type is integer
  if (op == "&" || op == "|" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=")
  {
    constraintHandler->handle(astToVar(element), boolType);
  }
  else if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%")
  {
    constraintHandler->handle(astToVar(element), intType);
  }

  if (op == "&" || op == "|")
  {
    // operands are integer
    constraintHandler->handle(astToVar(element->getLeft()), boolType);
    constraintHandler->handle(astToVar(element->getRight()), boolType);
  }
  else if (op != "==" && op != "!=")
  {
    // operands are integer
    constraintHandler->handle(astToVar(element->getLeft()), intType);
    constraintHandler->handle(astToVar(element->getRight()), intType);
  }
  else
  {
    // operands have the same type
    constraintHandler->handle(astToVar(element->getLeft()),
                              astToVar(element->getRight()));
  }
}

/*! \brief Type constraints for input statement.
 *
 * Type rules for "input":
 *  [[input]] = int
 */
void TypeConstraintVisitor::endVisit(ASTInputExpr *element)
{
  constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
}

/*! \brief Type constraints for function application.
 *
 * Type Rules for "E(E1, ..., En)":
 *  [[E]] = ([[E1]], ..., [[En]]) -> [[E(E1, ..., En)]]
 */
void TypeConstraintVisitor::endVisit(ASTFunAppExpr *element)
{
  std::vector<std::shared_ptr<TipType>> actuals;
  for (auto &a : element->getActuals())
  {
    actuals.push_back(astToVar(a));
  }
  constraintHandler->handle(
      astToVar(element->getFunction()),
      std::make_shared<TipFunction>(actuals, astToVar(element)));
}

/*! \brief Type constraints for heap allocation.
 *
 * Type Rules for "alloc E":
 *   [[alloc E]] = &[[E]]
 */
void TypeConstraintVisitor::endVisit(ASTAllocExpr *element)
{
  constraintHandler->handle(
      astToVar(element),
      std::make_shared<TipRef>(astToVar(element->getInitializer())));
}

/*! \brief Type constraints for address of.
 *
 * Type Rules for "&X":
 *   [[&X]] = &[[X]]
 */
void TypeConstraintVisitor::endVisit(ASTRefExpr *element)
{
  constraintHandler->handle(
      astToVar(element), std::make_shared<TipRef>(astToVar(element->getVar())));
}

/*! \brief Type constraints for pointer dereference.
 *
 * Type Rules for "*E":
 *   [[E]] = &[[*E]]
 */
void TypeConstraintVisitor::endVisit(ASTDeRefExpr *element)
{
  constraintHandler->handle(astToVar(element->getPtr()),
                            std::make_shared<TipRef>(astToVar(element)));
}

/*! \brief Type constraints for null literal.
 *
 * Type Rules for "null":
 *   [[null]] = & \alpha
 */
void TypeConstraintVisitor::endVisit(ASTNullExpr *element)
{
  constraintHandler->handle(
      astToVar(element),
      std::make_shared<TipRef>(std::make_shared<TipAlpha>(element)));
}

/*! \brief Type rules for assignments.
 *
 * Type rules for "E1 = E":
 *   [[E1]] = [[E2]]
 *
 * Type rules for "*E1 = E2":
 *   [[E1]] = &[[E2]]
 *
 * Note that these are slightly more general than the rules in the SPA book.
 * The first allows for record expressions on the left hand side and the second
 * allows for more complex assignments, e.g., "**p = &x"
 */
void TypeConstraintVisitor::endVisit(ASTAssignStmt *element)
{
  // If this is an assignment through a pointer, use the second rule above
  if (auto lptr = dynamic_cast<ASTDeRefExpr *>(element->getLHS()))
  {
    constraintHandler->handle(
        astToVar(lptr->getPtr()),
        std::make_shared<TipRef>(astToVar(element->getRHS())));
  }
  else
  {
    constraintHandler->handle(astToVar(element->getLHS()),
                              astToVar(element->getRHS()));
  }
}

/*! \brief Type constraints for while loop.
 *
 * Type rules for "while (E) S":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTWhileStmt *element)
{
  constraintHandler->handle(astToVar(element->getCondition()),
                            std::make_shared<TipBool>());
}

/*! \brief Type constraints for for loop.
 *
 * Type rules for "for (V : E1 .. E2 by E3) S":
 *   [[V]] = [[E1]] = [[E2]] = [[E3]] = int
 *
 * If the step is not provided, a constraint is not generated for it.
 */
void TypeConstraintVisitor::endVisit(ASTForLoopStmt *element)
{
  constraintHandler->handle(astToVar(element->getVar()),
                            std::make_shared<TipInt>());
  constraintHandler->handle(astToVar(element->getStart()),
                            std::make_shared<TipInt>());
  constraintHandler->handle(astToVar(element->getEnd()),
                            std::make_shared<TipInt>());
  if (element->getStep())
  {
    constraintHandler->handle(astToVar(element->getStep()),
                              std::make_shared<TipInt>());
  }
}

/*! \brief Type constraints for iterator loop.
 *
 * Type rules for "for (V in E) S":
 *   [[V]] = type of elements in [[E]]  -->  [[E]] = array([[V]])
 *
 * Since V is an iterator, the type of V should be the same as the type of elements in the array that E holds.
 * Thus, the type of V should be the same as the type that the array holds.
 * This means that the type of an array that has V as an element should be the same as the type the E array.
 */
void TypeConstraintVisitor::endVisit(ASTIterStmt *element)
{
  auto elementType = astToVar(element->getElement());
  auto arrayTypeVar = std::make_shared<SipArray>(elementType);
  auto arrayType = astToVar(element->getIterable());
  constraintHandler->handle(arrayType, arrayTypeVar);
}

/*! \brief Type constraints for if statement.
 *
 * Type rules for "if (E) S1 else S2":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTIfStmt *element)
{
  constraintHandler->handle(astToVar(element->getCondition()),
                            std::make_shared<TipBool>());
}

/*! \brief Type constraints for ternary expressions.
 *
 * Type rules for "E ? E1 : E2":
 *   [[E1]] = [[E2]] = [[E ? E1 : E2]]
 *   [[E]] = bool
 */
void TypeConstraintVisitor::endVisit(ASTTernaryExpr *element)
{
  constraintHandler->handle(astToVar(element->getCondition()),
                            std::make_shared<TipBool>());
  constraintHandler->handle(astToVar(element->getTrueExpr()),
                            astToVar(element->getFalseExpr()));
  constraintHandler->handle(astToVar(element),
                            astToVar(element->getTrueExpr()));
}

/*! \brief Type constraints for output statement.
 *
 * Type rules for "output E":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTOutputStmt *element)
{
  constraintHandler->handle(astToVar(element->getArg()),
                            std::make_shared<TipInt>());
}

/*! \brief Type constraints for array initializations.
 *
 * Type rule for "[ E1, ..., En ]":
 *   [[ [ E1, ..., En ] ]] = v[v]
 * where v = [[Ei]] and all Ei are the same type
 *
 * In the type construction v[v], the outer v is the type that the array holds and the inner v is the type of the elements
 * This type construction is redundant but it is used to make the type system more expressive to differentiate array type vs. array element type
 * in the case that different types within the array are allowed/decided in the future
 */
void TypeConstraintVisitor::endVisit(ASTArrayExpr *element)
{
  std::vector<std::shared_ptr<TipType>> elementTypes;
  std::shared_ptr<TipType> elementsType;

  if (element->ITEMS.empty())
  {
    // empty array
    elementTypes.push_back(std::make_shared<TipAlpha>(nullptr));
    constraintHandler->handle(astToVar(element),
                              std::make_shared<SipArray>(elementTypes));
    return;
  }

  bool first = true;
  std::shared_ptr<TipType> prevElementType;
  for (auto &e : element->ITEMS)
  {
    if (first)
    {
      prevElementType = astToVar(e.get());
      first = false;
    }
    else
    {
      prevElementType = astToVar(e.get());
      constraintHandler->handle(astToVar(e.get()), prevElementType);
    }
    elementTypes.push_back(astToVar(e.get()));
  }
  // elementsType = prevElementType;

  constraintHandler->handle(astToVar(element),
                            std::make_shared<SipArray>(elementTypes));
}

/*! \brief Type constraints for array of initializations.
 *
 * Type rule for "[ E1 of En ]":
 *   [[ [ E1 of E2 ] ]] = v[v]
 * where  v = [[E2]]
 *
 * E1 is the length of the array and must be an integer and E2 are the items which can be of any type
 * Thus, we need to handle the length and items separately
 */
void TypeConstraintVisitor::endVisit(ASTArrayOfExpr *element)
{
  std::shared_ptr<TipType> lengthType;
  std::shared_ptr<TipType> elementType;

  lengthType = astToVar(element->getLength());
  constraintHandler->handle(lengthType, std::make_shared<TipInt>());
  elementType = astToVar(element->getElement());

  constraintHandler->handle(astToVar(element),
                            std::make_shared<SipArray>(elementType));
}

/*! \brief Type constraints for array reference/indexing expression.
 *
 * Type rule for "E1[E2]":
 *   [[ E1[E2] ]] = type of elements in [[E1]]  -->  [[E1]] = array([[ E1[E2] ]])
 *
 * Since E1[E2] essentially results in an element in E1 array, the type of E1[E2] should be the same as the type of
 * elements in E1 or the type that the array holds.
 *
 * Thus, the type of an array that has E1[E2] as an element should be the same as the type E1 which contains similar type elements.
 */
void TypeConstraintVisitor::endVisit(ASTArrayRefExpr *element)
{
  std::shared_ptr<TipType> indexType;
  std::shared_ptr<TipType> arrayType;

  indexType = astToVar(element->getIndex());
  constraintHandler->handle(indexType, std::make_shared<TipInt>());
  arrayType = astToVar(element->getArray());

  std::shared_ptr<TipType> resultType = astToVar(element);
  auto arrayTypeVar = std::make_shared<SipArray>(resultType);
  constraintHandler->handle(arrayType, arrayTypeVar);

  if (auto sipArray = std::dynamic_pointer_cast<SipArray>(arrayType))
  {
    constraintHandler->handle(resultType, sipArray->type);
  }
}

/*! \brief Type constraints for unary expressions.
 *
 * Type rule for "op E" and "E op"; examples: "#E", "!E", "-E", "E++", "E--":
 *   [[ op E ]] = [[ E ]]
 *   [[ E op ]] = [[ E ]]
 *
 * In the case of "#E", "-E", "E++", and "E--", the type of E must be an integer
 * In the case of "!E", the type of E must be a boolean
 */
void TypeConstraintVisitor::endVisit(ASTUnaryExpr *element)
{
  if (element->getOp() == "#")
  {
    constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
  }
  else if (element->getOp() == "!")
  {
    constraintHandler->handle(astToVar(element), std::make_shared<TipBool>());
    constraintHandler->handle(astToVar(element->getExpr()), std::make_shared<TipBool>());
  }
  else if (element->getOp() == "-")
  {
    constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
    constraintHandler->handle(astToVar(element->getExpr()), std::make_shared<TipInt>());
  }
  else if (element->getOp() == "++" || element->getOp() == "--")
  {
    constraintHandler->handle(astToVar(element), std::make_shared<TipInt>());
    constraintHandler->handle(astToVar(element->getExpr()), std::make_shared<TipInt>());
  }
}

/*! \brief Type constraints for record expression.
 *
 * Type rule for "{ X1:E1, ..., Xn:En }":
 *   [[{ X1:E1, ..., Xn:En }]] = { f1:v1, ..., fn:vn }
 * where fi is the ith field in the program's global record
 * and vi = [[Ei]] if fi = Xi and \alpha otherwise
 */
void TypeConstraintVisitor::endVisit(ASTRecordExpr *element)
{
  auto allFields = symbolTable->getFields();
  std::vector<std::shared_ptr<TipType>> fieldTypes;
  for (auto &f : allFields)
  {
    bool matched = false;
    for (auto &fe : element->getFields())
    {
      if (f == fe->getField())
      {
        fieldTypes.push_back(astToVar(fe->getInitializer()));
        matched = true;
        break;
      }
    }
    if (matched)
      continue;

    fieldTypes.push_back(std::make_shared<TipAbsentField>());
  }
  constraintHandler->handle(astToVar(element),
                            std::make_shared<TipRecord>(fieldTypes, allFields));
}

/*! \brief Type constraints for field access.
 *
 * Type rule for "E.X":
 *   [[E]] = { f1:v1, ..., fn:vn }
 * where fi is the ith field in the program's global record
 * and vi = [[E.X]] if fi = X and \alpha otherwise
 */
void TypeConstraintVisitor::endVisit(ASTAccessExpr *element)
{
  auto allFields = symbolTable->getFields();
  std::vector<std::shared_ptr<TipType>> fieldTypes;
  for (auto &f : allFields)
  {
    if (f == element->getField())
    {
      fieldTypes.push_back(astToVar(element));
    }
    else
    {
      fieldTypes.push_back(std::make_shared<TipAlpha>(element, f));
    }
  }
  constraintHandler->handle(astToVar(element->getRecord()),
                            std::make_shared<TipRecord>(fieldTypes, allFields));
}

/*! \brief Type constraints for error statement.
 *
 * Type rules for "error E":
 *   [[E]] = int
 */
void TypeConstraintVisitor::endVisit(ASTErrorStmt *element)
{
  constraintHandler->handle(astToVar(element->getArg()),
                            std::make_shared<TipInt>());
}
