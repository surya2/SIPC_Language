grammar TIP;
// Grammar for Moeller and Schwartzbach's Tiny Imperative Language (TIP)

////////////////////// TIP Programs ////////////////////////// 

program : (function)+
;

function : nameDeclaration 
           '(' (nameDeclaration (',' nameDeclaration)*)? ')'
           KPOLY?
           '{' (declaration*) (statement*) returnStmt '}' 
;

////////////////////// TIP Declarations ///////////////////////// 

declaration : KVAR nameDeclaration (',' nameDeclaration)* ';' ;

nameDeclaration : IDENTIFIER ;

////////////////////// TIP Expressions ////////////////////////// 

// Expressions in TIP are ordered to capture precedence.
// We adopt the C convention that orders operators as:
//   postfix, unary, multiplicative, additive, relational, and equality 
//
// NB: # creates rule label that can be accessed in visitor
//
// ANTLR4 can automatically refactor direct left-recursion so we
// place all recursive rules as options in a single rule.  This
// means that we have some complex rules here that might otherwise
// be separated out, e.g., funAppExpr, and that we can't factor out
// other useful concepts, e.g., defining a rule for the subset of
// expressions that can be used as an l-value.  We prefer a clean 
// grammar, which simplifies AST construction, and work around these
// issues elsewhere in the compiler, e.g.,  introducing an assignable expr
// weeding pass. 
//
expr : expr '(' (expr (',' expr)*)? ')' 	#funAppExpr
     | expr '.' IDENTIFIER 			#accessExpr
     | expr (INC | DEC)         #incExpr
     | '*' expr 				#deRefExpr
     | SUB NUMBER				#negExpr
     | NOT+ expr                 #notExpr
     | prefix=SUB expr        #negNumExpr
     | '&' expr					#refExpr
     | expr op=(MUL | DIV | MOD) expr 		#multiplicativeExpr
     | expr op=(ADD | SUB) expr 		#additiveExpr
     | expr op=(GT | LT | GTE | LTE) expr 				#relationalExpr
     | expr op=(EQ | NE) expr 			#equalityExpr
     | expr op=AND expr            #andExpr
     | expr op=OR expr             #orExpr
     | <assoc=right> expr op=TIF expr op=TELSE expr #ternaryExpr
     | <assoc=right> expr op=(ASSIGN | CONCAT) expr #assignExpression
     | IDENTIFIER				#varExpr
     | NUMBER					#numExpr
     | KINPUT					#inputExpr
     | KALLOC expr				#allocExpr
     | KNULL					#nullExpr
     | TRUE                     #booleanTrueExpr
     | FALSE                    #booleanFalseExpr
     | recordExpr				#recordRule
     | '(' expr ')'				#parenExpr
     | LEN expr                 #lenExpr
     | array                    #arrayExpr
     | IDENTIFIER '[' expr ']'  #arrayRefExpr
;

recordExpr : '{' (fieldExpr (',' fieldExpr)*)? '}' ;

fieldExpr : IDENTIFIER ':' expr ;

////////////////////// TIP Statements ////////////////////////// 

statement : blockStmt
    | assignStmt
    | whileStmt
    | iterStmt
    | forStmt
    | ifStmt
    | outputStmt
    | errorStmt
;

assignStmt : expr '=' expr ';' ;

blockStmt : '{' (statement*) '}' ;

whileStmt : KWHILE '(' expr ')' statement ;

iterStmt : KFOR '(' expr ':' expr ')' statement ;

forStmt : KFOR '(' expr ':' expr '..' expr ('by' expr)? ')' statement ;

ifStmt : KIF '(' expr ')' statement (KELSE statement)? ;

outputStmt : KOUTPUT expr ';'  ;

errorStmt : KERROR expr ';'  ;

returnStmt : KRETURN expr ';'  ;

array : '[' ( expr ( ',' expr )* )? ']' ;

////////////////////// TIP Lexicon ////////////////////////// 

// By convention ANTLR4 lexical elements use all caps

MUL : '*' ;
DIV : '/' ;
MOD : '%' ;
ADD : '+' ;
SUB : '-' ;
GT  : '>' ;
GTE : '>=' ;
LT  : '<' ;
LTE : '<=' ;
EQ  : '==' ;
NE  : '!=' ;
TIF : '?'  ;
TELSE : ':' ;
CONCAT : '+=' ;
ASSIGN : '=' ;
LEN : '#' ;
INC : '++' ;
DEC : '--' ;

NUMBER : [0-9]+ ;

// Placing the keyword definitions first causes ANTLR4 to prioritize
// their matching relative to IDENTIFIER (which comes later).
KALLOC  : 'alloc' ;
KINPUT  : 'input' ;
KWHILE  : 'while' ;
KFOR    : 'for' ;
KIF     : 'if' ;
KELSE   : 'else' ;
KVAR    : 'var' ;
KRETURN : 'return' ;
KNULL   : 'null' ;
KOUTPUT : 'output' ;
KERROR  : 'error' ;
OR      : 'or' ;
AND     : 'and' ;
NOT     : 'not' ;
TRUE    : 'true' ;
FALSE   : 'false' ;

// Keyword to declare functions as polymorphic
KPOLY   : 'poly' ;

IDENTIFIER : [a-zA-Z_][a-zA-Z0-9_]* ;

// ANTLR4 has a nice mechanism for specifying the characters that should
// skipped during parsing.  You write "-> skip" after the pattern and
// let ANTLR4s pattern matching do the rest.

// Ignore whitespace
WS : [ \t\n\r]+ -> skip ;

// This does not handle nested block comments.
BLOCKCOMMENT: '/*' .*? '*/' -> skip ;

COMMENT : '//' ~[\n\r]* -> skip ;
