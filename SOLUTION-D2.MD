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

## Additional Testing 
After implimenting all of our nodes, we returned to the deliverable requirements, and concluded that we were missing some of the testing that was nessicary, most notably some coverage of `getChildren`, and the various kinds of print tests.

### Print Tests
We wanted to isolate single elements, so we used a template or a for loop printer test that iterated through statements as our template. Many of the tests ended up being trivial plug and play with this format, but a handful required more thoughtful design. The main consideration was deciding how the for loop would identify the target elements, which was a much harder task for non-statements. The procedure that ended up working in most cases was to encapsulate the element in a statement like an if, or an assign statement, and use the getters in those nodes to retrieve our target elements.

In a few cases, testing also helped us catch errors or areas for design improvements in the print function. One example is for loops, which outputted without separation between numbers, like `for(1 6 9)` which we decided to modify to more closely match the original syntax. Another was the ternary statement, who's expressions were missing a derefrence, and outputted memory addresses instead.
