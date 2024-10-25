# Deliverable 2 Solution Design

## General Process

For each new AST node, we followed the following steps to impliment it:

1. Create .h and .cpp files for the node, including print, accept, and getChildren funtions. Add the .h file to AST.h
2. Write the visit function in the ASTBuilder.cpp, and add the header to ASTBuilder.h
3. Add visit and endVisit in ASTVisitor.h
4. Add path to your node source files in CMakeLists.txt h
5. Add the codeGenFunction.cpp function for your node
6. Write a test in ASTNodeTests.cpp

The majority of new features followed the straighforward implimentation pattern with no addtional holdups. However, we did end up needing to make some interesting design descions regarding what is grouped into a single AST node, like putting all of the unary expressions together, but breaking for loops into `For` and `Iter`. These design decisions were largely motived by structure more than functionality; you could argue that `For` and `Iter` are more similar in function than `a++` and `not a` are, but the structure of `For` and `Iter` have more differences than the structure of the two unary statements. Thus, `a++` and `not a` used the same ast node `ASTUnaryExpr` while `For` and `Iter` statements were represented in completely different AST nodes. Additionally, if a programmer chooses to write an incremement decrement as a statement like `y++` instead of an expression like `y = x++`, a separate AST node called `ASTIncDecStmt` was created.

The project management steps for creating new AST nodes was written and strictly followed in this document: [Project Workflow Doc](https://docs.google.com/document/d/1PXQohzY-OZm6C1nks8-A24MVpECSDDFlni2qSiW1vEs/edit?usp=sharing)

## Node Testing

The node tests for the SIP language AST nodes were written in _SipcASTNodeTests.cpp_. After implementing each node, we wrote a test to ensure that the an input parse stream and resulting syntax tree produced a valid form of our AST Node. Each AST Node we created had a getter method which returned a `std::shared_ptr` reference to a child node (e.g. the iteration for loop has getter method to return a pointer to a variable expression AST node and an iterable expression AST node). To ensure that AST Node we just wrote has the correct attributes (correct child nodes), an input stream was passed into the `build_ast()` method, which calls ASTBuilder and its visit functions in the background. This creates an AST Program root node and an AST tree. Using the `find_node<ASTNode>()` method, the node that we just implemented could be found for the corresponding input stream and the getter methods retrieved each child node. The value/content of each child node was checked against an expected value. There are some cases, such as in the for loop statement, where there could be multiple AST Node configurations depending on the input stream. For example, if the input stream constructs a for loop like `for(i : x .. y)` without the `by <step amount>` at the end, then the `ASTForLoopStmt` type node would have 3 children and the corresponding test should check that there are only 3 non-null children with the correct value and the 4th child, in this case the step amount, is assigned to a null pointer. The node tests are preliminary tests to ensure that AST nodes are contstructed with valid structure and functionality.

**Note:** Our implementation of `ASTBuilder.cpp` converts `and`, `or`, and `not` operators in a given input stream to `&`, `|`, and `!` operator string representations respectively for their corresponding AST nodes. Additionally, boolean keywords are stored as C++ _bool_ types in `ASTBooleanExpr` AST node.

## Additional Testing

After implimenting all of our nodes, we returned to the deliverable requirements, and concluded that we were missing some of the testing that was necessary, most notably coverage of the `getChildren()` implemented in each AST node which contains child nodes, as well as various kinds of print tests. For coverage of `getChildren()` and to ensure correct functionality, we decided to include this test in _SipcASTNodeTests.cpp_. Our reasoning is because the _getChildren()_ method is really a check to make sure valid form of an AST node, in that it ensures that the right number and type of children were picked up by the current node. Thus, our node tests called the _getChildren()_ method of the respective nodes and ensured that the right number of children is returned from the method. In the case of testing a normal array construction like `x = [E1, E2, E3, ..., En]`, the _getChildren()_ test ensured that the method's return vector had the same amount of children as the _LEN_ attribute that all ASTArrayExpr nodes have. This ensured that trhe array ast nodes are constructed correctly in the ASTBuilder.

## Print Tests

We wanted to isolate single elements, so we used a template or a for loop printer test that iterated through statements, used `dynamic_cast<ASTStmt *>` to cast the current statement in the loop to the target AST node. If the dynamic cast yielded a valid cast, then we tested the print of the statement to see if it matched the expected statement in our array of expected priints. Many of the tests ended up being trivial plug and play with this format, but a handful required more thoughtful design. The main consideration was deciding how the for loop would identify the target elements, which was a much harder task for non-statements. The procedure that ended up working in most cases was to encapsulate the element in a statement like an if, or an assign statement, and use the getters in those nodes to retrieve our target elements.

In a few cases, testing also helped us catch errors or areas for design improvements in the print function. One example is for loops, which outputted without separation between numbers, like `for(1 6 9)` which we decided to modify to more closely match the original syntax. Another was the ternary statement, whose expressions were missing a derefrence and outputted memory addresses instead.

## Pretty Printer Tests

Our Pretty Printer tests were used for more than just checking the output of the `PrettyPrinter` visitor; it was also used to check the correct form of complicated statements and expressions like nested array constructions with complex expressions as elements, complex unary operations like dereferencing of getting length of an object resulting from an expression, complex binary expressions, and/or nested ternary operator expressions. In order to write these tests, we curated a set of complex expressions and wrote down the printed version on paper after reviewing the Pretty Printer indent levels, _indent()_ function, and how each expression is printed out. This formed our expected string result which we compared against the result of the _PrettyPrinter print()_ function. For example, for nested ternary expressions which are right associative in our TIP grammar, our PrettyPrinter `endVisit(ASTTernaryExpr *)` method unpacked the first ternary expression before visiting child ternary expressions. After constructing a print for the child ternary expressions, the child ternary expression print were combined into the final ternary expression print as so: `(condition) ? (child ternary expression in true case) : (child ternary expression in false case)`. Thus, our final pretty print output should have the same structure with the entire sub-child ternary expression being printed within the true or false case of the parent ternary expression.

### Function and Line Coverage

Our coverage report yielded _100%_ function coverage indicating that all functions of our AST nodes and `ASTBuilder.cpp` were being tested. Our line coverage yeilded on average _95%_ across all AST nodes due to a strange occurence where the last curly brace in the file was not being covered. Other than that, every non-trivial line is covered. _100%_ line coverage was recorded across the ASTBuilder functionality.
