#	Gear preprocessor documentation
##	Directives:
-	##	[@op](#1-op-directive)
-	##	[@join](#2-join-directive-lexprjoin-rexpr)
-	##	[@preinc](#3-preinc-directive-preinc-loc_pathpreinc-spec_path)
-	##	[@rawinc](#4-rawinc-directive-rawinc-loc_pathrawinc-spec_path)
-	##	[@save](#5-save-directive-save-expression)
-	##	[@def](#6-def-directive-def-name-expressiondef-namearguments-expression)
-	##	[@undef](#7-undef-directive-undef-macro_name)
-	##	[@ifdef](#8-ifdef-directive-ifdef-macro_name-expression)
-	##	[@ifndef](#9-ifndef-directive-ifndef-macro_name-expression)
-	##	[@if](#10-ifelif-directive-ifelif-condition-expression)
-	##	[@elif](#10-ifelif-directive-ifelif-condition-expression)
-	##	[@else](#11-else-directive-else-expression)
-	##	[@defined](#12-defined-directive-defined-macro_name)

##	1.	`@op` directive
###	Substitutes a character indicating a preprocessor directive
```"@op" -> "@"```

##	2.	`@join` directive (LEXPR`@join` REXPR)
###	Concatenates the left side of an expression with the right side
###	Parameters: `LEXPR` - left expression part, `REXPR` - right expression part
```"123@join 456" -> "123456"```

##	3.	`@preinc` directive (`@preinc` "LOC_PATH"/`@preinc` \<SPEC_PATH\>)
###	Includes the preprocessed content of the file at the specified path
###	Parameters: `LOC_PATH` - path to the file located in the same directory as the one being preprocessed, `SPEC_PATH` - path to a file located in one of the folders specified by the user (via the `-I` option)

##	4.	`@rawinc` directive (`@rawinc` "LOC_PATH"/`@rawinc` \<SPEC_PATH\>)
###	Includes the raw content of the file at the specified path
###	Parameters: `LOC_PATH` - path to the file located in the same directory as the one being preprocessed, `SPEC_PATH` - path to a file located in one of the folders specified by the user (via the `-I` option)

##	5.	`@save` directive (`@save` {EXPRESSION})
###	Does not preprocess data in the specified area
###	Parameters: `EXPRESSION` - data that does not need to be processed
```"@save {//	This text won't be removed :D}" -> "//	This text won't be removed :D"```

##	6.	`@def` directive (`@def` NAME {EXPRESSION}/`@def` NAME(ARGUMENTS) {EXPRESSION})
###	Defines a macro
###	Important: Macros cannot be used when using some preprocessor directives (`@preinc`, `@rawinc`, `@save`), you can specify parenthesized parameters that must follow the macro name, 
###	About `ellipsis`(`...`) operator: this operator in parameters expands the number of parameters to whatever is determined at the time the argument is used. When using `@ARG_INDEX` (ARG_INDEX - argument index) in the expression of a macro containing `...` in the parameters, the argument that corresponds to the given index in the `...` arguments will be substituted. When using `@` in a macro expression containing a `...` parameter, the number of arguments in the `...` arguments will be substituted.
```"@def NAME { 1 + 1 } /*...*/ NAME" -> " 1 + 1 "```

```"@def PRINTF(format, ...) {printf(format, ...)}/*...*/PRINTF("Integer = %d\n", 123)" -> "printf("Integer = %d\n", 123)"```

```"@def NUM_ARGS(...) {@}/*...*/NUM_ARGS(123, "a", 4348, 0xffff)" -> "4"```

```"@def EXRA_ARGS(a, ...) {a + @1}/*...*/EXTRA_ARGS(1, 2, 3)" -> "1 + 3"```

##	7.	`@undef` directive (`@undef` MACRO_NAME)
###	Deletes a macro
###	Parameters:	`MACRO_NAME` - macro name

##	8.	`@ifdef` directive (`@ifdef` MACRO_NAME {EXPRESSION})
###	Substitutes expression if macro is defined

##	9.	`@ifndef` directive (`@ifndef` MACRO_NAME {EXPRESSION})
###	Substitutes expression if macro is not defined

##	10.	`@if`/`@elif` directive (`@if`/`@elif` (CONDITION) {EXPRESSION})
###	Substitutes expression if condition is true
###	The `@elif` directive can only be used after the `@ifdef`, `@ifndef`, `@if`, `@elif` directives
###	If you want to check a complex condition, then use the operators `&`, `|` (don't forget about brackets of simple conditions)
```"@if ((3 % 5) & (1 + 2)) {@preinc \"expr.gear\"}" -> 3 % 5 = 3; 1 + 2 = 3; 3 & 3 = 1 => "@preinc \"expr.gear\""```

##	11.	`@else` directive (`@else` {EXPRESSION})
###	Substitutes the expression if the previous condition was false
```"@if (0) {@preinc \"0.gear\"} @else {@preinc \"1.gear\"}" -> "@preinc \"1.gear\""```

##	12.	`@defined` directive (`@defined` MACRO_NAME)
###	Substitutes 1 if the macro is defined, otherwise 0