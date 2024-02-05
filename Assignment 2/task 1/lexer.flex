%top {
    #include "parser.tab.hh"
    #define YY_DECL yy::parser::symbol_type yylex()
    #include "Node.h"
    int lexical_errors = 0;
}

%option yylineno noyywrap nounput batch noinput stack
IDENTIFIER [a-zA-Z][a-zA-Z0-9_]*
INTEGER_LITERAL 0|[1-9][0-9]*
%%

"class" {if(USE_LEX_ONLY) {printf("CLASS\n");} else {return yy::parser::make_CLASS(yytext);}}
"{" {if(USE_LEX_ONLY) {printf("LBRACE\n");} else {return yy::parser::make_LBRACE(yytext);}}
"}" {if(USE_LEX_ONLY) {printf("RBRACE\n");} else {return yy::parser::make_RBRACE(yytext);}}
"(" {if(USE_LEX_ONLY) {printf("LPAR\n");} else {return yy::parser::make_LPAR(yytext);}}
")" {if(USE_LEX_ONLY) {printf("RPAR\n");} else {return yy::parser::make_RPAR(yytext);}}
"[" {if(USE_LEX_ONLY) {printf("LBRACKET\n");} else {return yy::parser::make_LBRACKET(yytext);}}
"]" {if(USE_LEX_ONLY) {printf("RBRACKET\n");} else {return yy::parser::make_RBRACKET(yytext);}}
";" {if(USE_LEX_ONLY) {printf("SC\n");} else {return yy::parser::make_SC(yytext);}}
"=" {if(USE_LEX_ONLY) {printf("EQASSIGN\n");} else {return yy::parser::make_EQASSIGN(yytext);}}
"!" {if(USE_LEX_ONLY) {printf("NOT\n");} else {return yy::parser::make_NOT(yytext);}}
"<" {if(USE_LEX_ONLY) {printf("LT\n");} else {return yy::parser::make_LT(yytext);}}
">" {if(USE_LEX_ONLY) {printf("GT\n");} else {return yy::parser::make_GT(yytext);}}
"&&" {if(USE_LEX_ONLY) {printf("AND\n");} else {return yy::parser::make_AND(yytext);}}
"||" {if(USE_LEX_ONLY) {printf("OR\n");} else {return yy::parser::make_OR(yytext);}}
"==" {if(USE_LEX_ONLY) {printf("EQ\n");} else {return yy::parser::make_EQ(yytext);}}
"+" {if(USE_LEX_ONLY) {printf("ADDSIGN\n");} else {return yy::parser::make_ADDSIGN(yytext);}}
"-" {if(USE_LEX_ONLY) {printf("SUBSIGN\n");} else {return yy::parser::make_SUBSIGN(yytext);}}
"*" {if(USE_LEX_ONLY) {printf("MULSIGN\n");} else {return yy::parser::make_MULSIGN(yytext);}}
"/" {if(USE_LEX_ONLY) {printf("DIVSIGN\n");} else {return yy::parser::make_DIVSIGN(yytext);}}
"." {if(USE_LEX_ONLY) {printf("DOT\n");} else {return yy::parser::make_DOT(yytext);}}
"," {if(USE_LEX_ONLY) {printf("COMMA\n");} else {return yy::parser::make_COMMA(yytext);}}
"public" {if(USE_LEX_ONLY) {printf("PUBLIC\n");} else {return yy::parser::make_PUBLIC(yytext);}}
"static" {if(USE_LEX_ONLY) {printf("STATIC\n");} else {return yy::parser::make_STATIC(yytext);}}
"void" {if(USE_LEX_ONLY) {printf("VOID\n");} else {return yy::parser::make_VOID(yytext);}}
"main" {if(USE_LEX_ONLY) {printf("MAIN\n");} else {return yy::parser::make_MAIN(yytext);}}
"boolean" {if(USE_LEX_ONLY) {printf("BOOLEAN\n");} else {return yy::parser::make_BOOL(yytext);}}
"String" {if(USE_LEX_ONLY) {printf("STRING\n");} else {return yy::parser::make_STRING(yytext);}}
"int" {if(USE_LEX_ONLY) {printf("INT\n");} else {return yy::parser::make_INT(yytext);}}
"if" {if(USE_LEX_ONLY) {printf("IF\n");} else {return yy::parser::make_IF(yytext);}}
"else" {if(USE_LEX_ONLY) {printf("ELSE\n");} else {return yy::parser::make_ELSE(yytext);}}
"while" {if(USE_LEX_ONLY) {printf("WHILE\n");} else {return yy::parser::make_WHILE(yytext);}}
"System.out.println" {if(USE_LEX_ONLY) {printf("PRINTLN\n");} else {return yy::parser::make_PRINTLN(yytext);}}
"length" {if(USE_LEX_ONLY) {printf("LENGTH\n");} else {return yy::parser::make_LENGTH(yytext);}}
"return" {if(USE_LEX_ONLY) {printf("RETURN\n");} else {return yy::parser::make_RETURN(yytext);}}
"true" {if(USE_LEX_ONLY) {printf("TRUE\n");} else {return yy::parser::make_TRUE(yytext);}}
"false" {if(USE_LEX_ONLY) {printf("FALSE\n");} else {return yy::parser::make_FALSE(yytext);}}
"this" {if(USE_LEX_ONLY) {printf("THIS\n");} else {return yy::parser::make_THIS(yytext);}}
"new" {if(USE_LEX_ONLY) {printf("NEW\n");} else {return yy::parser::make_NEW(yytext);}}

{INTEGER_LITERAL} {
   return yy::parser::make_INT_LITERAL(yytext);
}

{IDENTIFIER} {
   return yy::parser::make_ID(yytext);
}


[\t\n ]+ {
}

"//"[^\n]* {
}
.                       { if(!lexical_errors) fprintf(stderr, "Lexical errors found! See the logs below: \n"); fprintf(stderr,"\t@error at line %d. Character %s is not recognized\n", yylineno, yytext); lexical_errors = 1;}
<<EOF>>                  {return yy::parser::make_END();}
%%