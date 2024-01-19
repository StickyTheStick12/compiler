%top {
#include "stdlib.h"
int yyError(const char* s);
int bufferSize = 15;
char* buffer = NULL;
int nextPos = 0;
}

%option noyywrap nounput batch noinput stack
IDENTIFIER [a-zA-Z][a-zA-Z0-9_]*
COMMENT \/\/.*
INTEGER_LITERAL 0|-?[1-9][0-9]*

%%
"class" {
   printf("CLASS\n");
}

"{" {
   printf("LBRACE\n");
}

"}" {
   printf("RBRACE\n");
}

"(" {
   printf("LPAR\n");
}

")" {
   printf("RPAR\n");
}

"[" {
   printf("LBRACKET\n");
}

"]" {
   printf("RBRACKET\n");
}

";" {
   printf("SC\n");
}

"=" {
   printf("EQASSIGN\n");
}

"!" {
   printf("NOT\n");
}

"<" {
   printf("LT\n");
}

">" {
   printf("GT\n");
}

"&&" {
   printf("AND\n");
}

"||" {
   printf("OR\n");
}

"==" {
   printf("EQ\n");
}

"+" {
   printf("ADDSIGN\n");
}

"-" {
   printf("SUBSIGN\n");
}

"*" {
   printf("MULSIGN\n");
}

"/" {
   printf("DIVSIGN\n");
}

"." {
   printf("DOT\n");
}

"," {
   printf("COMMA\n");
}

"public" {
   printf("PUBLIC\n");
}

"static" {
   printf("STATIC\n");
}

"void" {
   printf("VOID\n");
}

"main" {
   printf("MAIN\n");
}

"extends" {
   printf("EXTENDS\n");
}

"boolean" {
   printf("BOOL\n");
}

"String" {
   printf("STRING\n");
}

"int" {
   printf("INT\n");
}

"if" {
   printf("IF\n");
}

"else" {
   printf("ELSE\n");
}

"while" {
   printf("WHILE\n");
}

"System.out.println" {
   printf("printLN\n");
}

"length" {
   printf("LENGTH\n");
}

"return" {
   printf("RETURN\n");
}

"true" {
   printf("TRUE\n");
}

"false" {
   printf("FALSE\n");
}

"this" {
   printf("THIS\n");
}

"new" {
   printf("NEW\n");
}

{INTEGER_LITERAL} {
   printf("INT_LITERAL: %s\n", yytext);
}

{IDENTIFIER} {
   printf("IDENTIFIER: %s\n", yytext);
}

{COMMENT} {
   printf("COMMENT: %s\n", yytext);
}

[\t\n ]+ {
}

. {
    if (buffer == NULL)
        buffer = (char*)malloc(15);

    buffer[nextPos] = yytext[0];
    nextPos++;

    if (nextPos == bufferSize)
    {
        bufferSize = bufferSize * 2;
        buffer = (char*)realloc(buffer, bufferSize);
    }
}

<<EOF>>  {
    if (buffer != NULL) {
        buffer[nextPos] = '\0';
        return yyError(buffer);
    }
    else
        return 0;
}
%%
int yyError(const char* str) {
   fprintf(stderr, "Unrecognized input: %s\n", str);
   return 0;
}
