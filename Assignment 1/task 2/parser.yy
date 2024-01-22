%skeleton "lalr1.cc"
%defines
%define parse.error verbose
%define api.value.type variant
%define api.token.constructor

%code requires{
  #include <string>
  #include "Node.h"
  #define USE_LEX_ONLY false //change this macro to true if you want to isolate the lexer from the parser.
}

%code{
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;

  Node* root;
  extern int yylineno;
}

// definition of set of tokens. All tokens are of type string
%token <std::string> CLASS LBRACE RBRACE LPAR RPAR LBRACKET RBRACKET SC EQASSIGN NOT LT GT AND OR EQ ADDSIGN SUBSIGN MULSIGN DIVSIGN DOT COMMA PUBLIC STATIC VOID MAIN EXTENDS BOOLEAN STRING INT IF ELSE WHILE PRINTLN LENGTH RETURN TRUE FALSE THIS NEW INT_LITERAL ID
%token END 0 "end of file"

//definition of operator precedence. See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl
%left ADDSIGN SUBSIGN
%left MULSIGN DIVSIGN

// definition of the production rules. All production rules are of type Node
%type <Node*> Goal MainClass ClassDeclaration ClassBody ClassDeclarationP ClassDeclarationVars VarDeclaration MethodDeclaration MethodParam MethodBody MethodDeclarationParams MethodDeclarationVars MethodDeclarationStatements Type Statement LRStatement Expression orExpression andExpression eqExpression lgExpression  multdivExpression  notExpression comExpression addsubExpression  parExpression  Identifier Int

%%

Goal:
    MainClass END { $$ = $1; root = $$; }
    | MainClass ClassDeclarationP END { $$ = $1; $$->children.push_back($2); root = $$; };

MainClass:
    CLASS Identifier LBRACE PUBLIC STATIC VOID MAIN LPAR STRING LBRACKET RBRACKET Identifier RPAR LBRACE Statement RBRACE RBRACE {
      $$ new Node("Main Class", "", yylineno);
      $$->children.push_back($2); // Identifier
      $$->children.push_back($12); // Identifier
      $$->children.push_back($15); // Statement
    };

ClassDeclarationList:
  Classdeclaration {
    $$ = new Node("ClassList", "", yylineno);
    $$->children.push_back($1);
  }
  | ClassDeclarationList Classdeclaration {
    $$ = $1;
    $$->children.push_back($1);
  };

ClassDeclaration:
  CLASS Identifier ClassBody {
    $$ = new Node("Class", "", yylineno);
    $$->children.push_back($2); // Identifier
    $$->children.push_back($3); // VarDeclaration
};




ClassBody: 
  LBRACE RBRACE {
    $$ = new Node("No class body", "", yylineno);
  }
  | LBRAVE ClassDeclarationVars RBRACE {
    $$ = $2;
  }
  | LBRACE ClassDeclarationMethods RBRACE {
    $$ = $2;
  }
  | LBRACE ClassDeclarationVars ClassDeclarationMethods RBRACE {
    $$ = new Node("Class body", "", yylineno);
    $$->children.push_back($2);
    $$->children.push_back($3);
};





