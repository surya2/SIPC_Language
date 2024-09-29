# SIPC - Extension of TIPC for SIP Language

## Language Description

The SIP language extends upon the TIP language with additional operators and constructs. SIP adds boolean constants and operators with the ability to set variables equal to 'true' or 'false' states as well as 'and' and 'or' operators to combine conditions within a statement such as an 'if' or 'while' loop. A 'not' unary operator is also included to extend the power of TIP, allowing more execution options such as executing expressions when logic resolves to false. For value comparison, the SIP language extends comparator operators with the >=, <, and <= operators.

## Thought Process

#### Preliminary Test-driven Development

To extend the TIP language parser, a test-driven process was utilized to write tests checking for parseability for each of the languages extensions described above. After writing each test, the TIP grammar (defined in the TIP.g4 file) was extended enough to allow the written tests to pass. To begin, we began writing tests for simple applications of the new extensions and checking whether the application can be interpretted by the parser. Below are examples of simple applications that were tested:

- _not x_
- _x and y_
- _x or y_
- _x = [1, 2]_
- _x = [x of 2]_
- _#x_
- _x % y_
- _x = -2_
- _x >= 2_, _x < 2_, and _x <= 2_
- _x ? y : z_
- _x++_ and _x--_
- _for(x : [1, 2, 3])_ and _for(i : x .. y by z)_

#### Operator Precedence thought-process and testing

After testing these simple applications, multiple simple applications were combined into a single, complex expression and parseability of these complex expressions was tested for. The aim of these tests was to test the rescursive parsing of the ANTLR grammar by nesting multiple operators in a single expression. Additionally, this allowed us to test operator precedence and ensure that the order of _expr_ tokens in the ANTLR grammar result in expected parse output. With much diussion, the grammar was written from the very beginning with precedence in mind as follows from highest precedence to lowest precedence:

- Unary Operators
  - _E++_ and _E--_ have equal precedence and requires higher precedence compared to arithmetic and boolean operators due to a situation like _x++ / 2_ in which _x_ must be incremented and assigned to itself before being divided by another expression.
  - _not x_ and _-2_ or _-x_ which all have roughly equal precedence.
- Binary Operators
  - Arithmetic Operators: As multiplication, division, and modulo (division but result as the remainder) are all the same precedence and included in the same line in the ANTLR grammar. Following this is subtraction and addition which have a lower order of operation.
  - Comparator Operators: >=, <, and <= were included in a single line but lower in precedence compared to arithmetic operators as arithmetic operators should be evaluated first in the case of a scenario like _x+2 >= y\*3_.
  - Boolean Short-circuiting Operators: Boolean binary operators _and_ and _or_ generally have lower preccedence compared to arithmetic operators. We reasoned that it is fine for them to be below the >, >=, <, and <= operators since these comparators cannot be used to compare boolean logic conditions. However, the boolean binary operators should have higher precedence applied for == and != operators which can be applied to boolean conditions.
- Ternary Operators/Expressions: Ternary operators should have lower precedence to arithmetic and boolean expressions given scenarios like _z = x+2 ? x > y : y+2_ or _z = true ? x and y : false_ in which case the arithmetic, comparator, and boolean binary operators should be valuated first before evaluating the logic of the ternary operators.
- Constants: Identifiers for vaiable names, numbers, and constants like 'true' and 'false' are included in the precedence hierarchy compared to aforementioned expressions due to their singular atomicity.
- Array and Record Constructions: Due to the fact that array constructions can have multiple expressions within as elements, it is included as the lowest expression in the precedence hierarchy with roughly equal precedence to records.

#### Testing of Precedence and Complex Expressions

The below types of complex expressions were tested for parseability to validate the precedence ordering of the grammar:

- _not x and y_ and _not x or y_
- _x and y or z_ which tested the right recursive nature of ANTLR in which _x and y_ should hold higher precedence even though _and_ and _or_ have equal precedence.
- _#[3, 4, 6, 7]_
- _x + y % z_
- _x _ y % z* which should similarly parse *x _ y_ with lower precedence.
- _-x + y_
- _x+2 <= y\*3_
- _z = x+2 ? x > y : y+2_

Additionally, order precedence was tested with greater integrity with a series of 20 expected parse tree tests in which the below expressions were inputted to the parser and the output was checked against an expected output in the parse tree:

- _z = x and y or x_
- _z = not x and y or x_
- _z = x and not y or x_
- _z = x % y + 1_
- _z = -x + y_
- _z = x > y ? x : y > 1 ? y : x_
- _z = x > y ? y > 1 ? y : x : 0_

We came across an issue with our at this stage in which a complex ternary operator like _z = x > y ? x : y > 1 ? y : x_ was not being parsed correctly. Our intuition for the above ternary expression is that the outer ternary operator which acts as a if-else statement includes a nested ternary or if-else statement. Thus, the above ternary operators evaluates to

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; if(x > y){<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; x<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;} else {<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; if(y > 1){<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; y<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; } else {<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; x<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; }<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}

Thus, the nested ternary operator should have higher precedence. For instance, the ternary grammar is _expr op=TIF expr op=TELSE expr_. When looking at the ternary expression _x > y ? x : y > 1 ? y : x_, the rightmost ternary operator _y > 1 ? y : x_ is the nested operator with higher precedence. We noticed a pattern where the right most expressions in a ternary operator should always be associated first or applied to higher precedence before reading the rest of the ternary operator. After much resarch, we found that ANTLR supports right-associativity using the <assoc=right> in front of the grammar. This allowed us to pass the ternary specific expected parse tree tests.

#### Testing for Not Parseable scenarios

The way we approached testing for non-parseable SIP code was by thinking about both simple syntax errors as well as common errors where developers may migrate from a different language and use constructs or operators common in the languages they are migrating from. Examples we tested for include unclosed for loop headers like _for i : x .. y by 2_ which is common in Python, illegal contructions like _x = {1, 2, 3};_ for arrays which is similar to Java, and illegal operators such as _!x_ which is the logic not operator like C/C++.
