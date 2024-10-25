# Deliverable 2 Solution Design

### General Process
For each new AST node, we followed the following steps to impliment it:
1. Create .h and .cpp files for the node, including print, accept, and getChildren funtions. Add the .h file to AST.h
2. Write the visit function in the ASTBuilder.cpp, and add the header to ASTBuilder.h
3. Add visit and endVisit in ASTVisitor.h
4. Add path to your node source files in CMakeLists.txt h
5. Add the codeGenFunction.cpp function for your node
6. Write a test in ASTNodeTests.cpp 

The majority of new features followed the straighforward implimentation pattern with no addtional holdups. However, we did end up needing to make some interesting design descions regarding what is grouped into a single AST node, like putting all of the unary expressions together, but breaking for loops into `For` and `Iter`. These design descions were largely motived by structure more than functionality; You could argue that `For` and `Iter` are more similair in function than `a++` and `not a` are, but the structure of  `For` and `Iter` have more differences than the structure of the two unary statements.

## Print Tests
After implimenting all of our nodes, we returned to the deliverable requirements, and concluded that we were missing some of the testing that was nessicary, most notably some coverage of `getChildren`, and the various kinds of print tests.