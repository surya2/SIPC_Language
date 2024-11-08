# Deliverable 3 Solution Design

## General Process
Rather than the AST nodes, which often followed very similar implementation structure for very different features, implementing semantics was far more varied across different feature types, depending on whether they needed entirely new concrete semantic types in the most difficult cases, or on the other end of the spectrum if they strictly followed with the semantics of existing feature categories, and only needed few line additions. With that said, some commonalities remained, like adapting `TypeConstraintVisitor.cpp` to accommodate type checking of the newly implemented features, and, as always creating new tests.

## Implementation Difficulties
Unsurprisingly, the most difficult feature addition was the one that required a new type, and the most complexity: the arrays. This complexity is added to by the presence of two different ways to initialize arrays: the traditional array literals and the "of" arrays. On top of just implementing the array types themselves, particular difficulty was paid to getting array indexs to work properly, specifically selecting types from inside an array, for example with `x = y[1]`, trying to get the type of `x` to be the same as the type of the item in index `[1]` of the array. 



## Testing
As usual, the general testing structure of the previous semantics tests mostly held for the new features. This broke down into two new testing files for the two new types (the two arrays), and a litany of new tests of type constraints for the remainder of new features added. Due to the uniform structure of many of these features (unary, binary, ternary, etc.), most of the type constraint tests were grouped into single tests, testing many features of similar structure. One thing that stood out was that the line coverage report of semantics seems a little more idiosyncratic than the front end, but this remained mostly unchanged, and the locations in the file structure with low line coverage were also uncovered before our additions, while the parts that are tested (i.e. `sematic/types/concrete`) achieved similar coverage levels with new tests
