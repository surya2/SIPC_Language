# Deliverable 3 Solution Design

## Improvements from Deliverable 2

One major imporvement we made from Deliverable 2 to make implementation easier for Deliverable 3 was to turn all Increment and Decrement statements such as `E++` and `E--` into a `ASTAssignStmt` node constaining a `ASTBinaryExpr` node as its right hand side expression such that they are represented as `E=E+1` and `E=E-1` respectively. This allowed us to avoid creating a type constraint for increment and decrement statements.

## General Process

Rather than the AST nodes, which often followed very similar implementation structure for very different features, implementing semantics was far more varied across different feature types, depending on whether they needed entirely new concrete semantic types in the most difficult cases, or on the other end of the spectrum if they strictly followed with the semantics of existing feature categories, and only needed few line additions. With that said, some commonalities remained, like adapting `TypeConstraintVisitor.cpp` to accommodate type checking of the newly implemented features, and, as always creating new tests.

The general process is outlined in this workflow [document](https://docs.google.com/document/d/1sMwRkiGzycrzBrHIaQn-7JFmhP84LtrqLAqr3SXR7D8/edit?usp=sharing).

### Identifying and Implementing Constraints

To begin, we identified the type constraints below:

- `ASTBooleanExpr` nodes created from `true` or `false` are expected to be constrained to some boolean type variable.
- `ASTArrayExpr` nodes are structures in the SIP language which have items in them. An important consideration that was thought of is that an array reference/index operation such as `E1[E2]` where `E1` is a variable representing an array and `E2` is an expression representing an index must have the type of an element in the array. Originally, the implementation allowed for arrays of any type. However, there is no way of knowing the type of `E1[E2]` if array `E1` can hold elements of multiple types. Thus, a constraint that all elements that are children of `ASTArrayExpr` must be the same type was applied. Please refer to comments above `endVisit(ASTArrayExpr *element)` in `TypeConstraintVisitor.cpp` for reference on constraints and building type constraint for the overall array.
- `ASTArrayOfExpr` nodes are structures in the SIP language which have a length expression and an element expression as follows: `[E1 of E2]` where `E1` is a length expression and `E2` is an element expression. A constraint to ensure `E1` is an `int` type is applied. The type of `E2` is used to construct the type of the _ArrayOf_ expression; please refer to the **DRY Type Class Implementation** section below for how a type for _ArrayOf_ expression is constructed.
- `ASTArrayRefExpr` is for array references or indexing and follows the expression structure `E1[E2]`. In this case, the array reference expression should return the same type as the type of elements in the array `E1`. This constraint is applied.
- `ASTUnaryExpr` nodes can return/result in multiple types depending on the operator involved. Each case of operator is handled in the `endVisit(ASTUnaryExpr *)` in `TypeConstraintVisitor.cpp`
- Similarly, `ASTBinaryExpr` nodes return different types depending on the operator involved and each case is handled in `TypeConstraintVisitor.cpp`. Additionally, the right hand nd left hand side expression need to have constraints of what kind of time it may be since operators such as `+` can only operate on `int` types while `and` can only operate on `bool` types.
- `ASTTernaryExpr` nodes follow the structure `(condition) ? (expression_returned_when_true) : (expression_return_when_false)`. Thus, the `(condition)` expression is to be constrained to some `bool` type. However, if the ternary expression could return any/different types for `(expression_returned_when_true)` and `expression_returned_when_false`, then it could noty be unified with the variable which is assigned to the ternary expression. Thus, a type constraint to ensure that `expression_returned_when_true` ihas same type as `expression_returned_when_false` is applied.
- `ASTForLoopStmt` has a clear structure in which inside a for loop header such as `for(V : S .. E by STEP)`, all expressions `V`, `S`, `E`, and `STEP` must be constrained to an `int` type.
- `ASTIterStmt` also had a clear structure in which inside iterator loop header `for(V : A)`, `A` is some `array` type that can be iterated on and `V` is the type of the elements in the array. A constraint to ensure `V` is the same as the types of elements in `A` is applied.

### Implementing TipType classes

After creating new `endVisit(ASTExpr *)` methods for each constraint detailed above, the types to be created were determined.

- Two new types were determined that variables coudl unify to: `bool` (boolean) and `array`. Each were called `TipBool` and `SipArray` respectively. Classes inheriting `TipCons` for each new type determined was created and `TipTypeVisitor` _visit()_ and _endVisit()_ methods were implemented.
- Additionally, in `Substituter.cpp`, methods to substitute all visits of `TipBool` and `SipArray` unifications were processed with a new type object for the variable assignment unified.

### DRY Type Class implementation

For `ASTArrayOfExpr`, a type is constructed using the existing `SipArray` _TipType_ class. This allowed us to uphold DRY principles in our code. `SipArray` only takes in either a vector of its element's types or a single type if a single element (or empty array with _alpha_ type) as its arguments. Thus, an _ArrayOf_ expression of the form `[E1 of E2]` can simply create a `SipArray` object/type variable, passing in the type of `E2` as an argument, and be represented just like any other array.

## Implementation Difficulties

The most challenging task was implementing array reference/index constraints, specifically selecting types from inside an array. For example with `x = y[1]`, trying to get the type of `x` to be the same as the type of the item in index `[1]` of the array was a challenge. The reason for this is because there is no way of creating a constraint on `y` to ensure that is an array type. This requires the following syntax:
```
constraintHandler->handle(
      astToVar(element->getArrayExpr()), 
      std::make_shared<SipArray>(
                        type_of_elements)
    );
```
However, there is no way of knowing what the type in the array `y` is because it would require going all the way back to where `y` is initialized and seeing what the type of the elements are, all under the assumption that unification of the elements inside `y` has already happened. Thus, a clever trick was thought of after much brainstorming: if `E1[E2]` is supposed to resolve to the type of the elements in `E1` array, then if we create a `SipArray` type variable with the type of `E1[E2]` passed into it (an array type that holds the type of `E1[E2]`), it should be the same type as the type of `E1` and can thus be constrained to type of `E1`.

This solution was used as inspiration for Iterator loops as well. In iterator loops like `for(V : A)` where `A` is an array type, `V` must hve the same type as the type of elements in `A`. Thus, the type of an array with elements of type of `V` should be the same as the type of `A`. That would ensure that `V` is constrained to having the same type as the elements in `A`.

## Testing

As usual, the general testing structure of the previous semantics tests mostly held for the new features. This broke down into two new testing files for the two new types (bool and array), and a litany of new tests of type constraints for the remainder of new features added. Due to the uniform structure of many of these features (unary, binary, ternary, etc.), most of the type constraint tests were grouped into single tests, testing many features of similar structure. 

Once a new *TipType* class was created, a new test with the same name as the class in `test/semantic/types/concrete` was created and each method except for the `accept(TipTypeVisitor *)` was tested. The `==` and `!=` tests were each tested. For *TipBool* for instance, it was compared with another *TipBool* object to test *==* and also tested against a different type altogether to test *!=* For *SipArray*, an object with different types was used to test *!=* functionality. All concrete tests also had output stream test cases to test the `print` method in each new *TipType* class.

Once all constraints were implemented, new test cases were written in `test/semantic/types/contraints/TypeConstraintCollectTest.cpp`. Each group of tests are distringuished by a comment header. After each constraint was implemented, a single, basic test was implemented to ensure that the implementation of teh constraint generally works. If notm then fixes were made to the constraint method and associated *TipType* class in a pseudo-test-driven development manner. After fixes were applied, the basic tests were run again to ensure functionality. After that, more rigorous tests were implemented. For instance for arrays, tests for arrays with record types or references was tested or functions returning arrays was tested to ensure that nested types could be inferred and function types could be inferred. A good rule of thumb that we used, especially with arrays and ternary expressions, was to first test with `int` elements and then write tests with more complicated types like records to see if the type inference holds.

Tests implemented in `test/semantic/types/concrete` allowed coverage of `==`, `!=`, and `print` methods while tests implemented in `test/semantic/types/contraints/TypeConstraintCollectTest.cpp`, although also covering `==` and `!=` through the `Unifier`, primarily covered the `accept(TipTypeVisitor *)` methods in each new *TipType*. 100% coverage was recoded amongst the new *Tiptype* classes implemented in `src/semantic/types/concrete`. It was noticed that `TipMu.cpp` did not have sufficient coverage. We tried implement a recursive function in `TypeConstraintCollectTest.cpp` but noticed that `TipMu` wasn't even applied as constraints in `TypeConstraintVisitor.cpp` and decided not to pursue it assuming there was a reason for lack of constraint tests for it.
