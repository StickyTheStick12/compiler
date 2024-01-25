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
%token <std::string> CLASS LBRACE RBRACE LPAR RPAR LBRACKET RBRACKET SC EQASSIGN NOT LT GT AND OR EQ ADDSIGN SUBSIGN MULSIGN DIVSIGN DOT COMMA PUBLIC STATIC VOID MAIN BOOL STRING INT IF ELSE WHILE PRINTLN LENGTH RETURN TRUE FALSE THIS NEW INT_LITERAL ID
%token END 0 "end of file"

//definition of operator precedence. See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl
%precedence "then"
%precedence ELSE
%right EQ
%left OR
%left AND
%left LT GT
%left ADDSIGN SUBSIGN
%left MULSIGN DIVSIGN
%left DOT
%left NOT

// definition of the production rules. All production rules are of type Node
%type <Node*> Goal MainClass StatementList ClassDeclarationList ClassDeclaration ClassBody VarDeclarationClassList VarDeclaration MethodDeclaration MethodBody MethodDeclarationParameter MethodDeclarationParameters VarDeclarationMethod MethodStatement Type Statement Statements Expression commaExpression Identifier Int

%%

Goal:
    MainClass END { $$ = $1; root = $$; }
    | MainClass ClassDeclarationList END {
        $$ = $1;
        $$->children.push_back($2);
        root = $$;
    };

MainClass:
    PUBLIC CLASS Identifier LBRACE PUBLIC STATIC VOID MAIN LPAR STRING LBRACKET RBRACKET Identifier RPAR LBRACE Statement StatementList RBRACE RBRACE {
      $$ = new Node("Main Class", "", yylineno);
      $$->children.push_back($3); // Identifier
      $$->children.push_back($13); // Identifier
      $$->children.push_back($16); // Statement
      //$$->children.push_back($17); // Statementlist
    };

StatementList:
    Statement {
        $$ = new Node("Main class statement", "", yylineno);
        $$->children.push_back($1);
    }
    | StatementList Statement {
        $$ = $1;
        $$->children.push_back($2);
    };

ClassDeclarationList:
  ClassDeclaration {
    $$ = new Node("ClassList", "", yylineno);
    $$->children.push_back($1);
  }
  | ClassDeclarationList ClassDeclaration {
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
    | LBRACE VarDeclarationClassList RBRACE {
        $$ = $2;
    }
    | LBRACE MethodDeclaration RBRACE {
        $$ = $2;
    }
    | LBRACE VarDeclarationClassList MethodDeclaration RBRACE {
        $$ = new Node("Class body", "", yylineno);
        $$->children.push_back($2);
        $$->children.push_back($3);
    };

VarDeclarationClassList:
    VarDeclaration {
        $$ = new Node("class variables", "", yylineno);
        $$->children.push_back($1);
    }
    | VarDeclarationClassList VarDeclaration {
        $$ = $1;
        $$->children.push_back($2);
    };

VarDeclaration:
    Type Identifier SC {
        $$ = new Node("Variable", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($2);
    };

MethodDeclaration:
  PUBLIC Type Identifier LPAR RPAR MethodBody {
    $$ = new Node("Method", "", yylineno);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->children.push_back($6);
  }
  | PUBLIC Type Identifier LPAR MethodDeclarationParameters RPAR MethodBody {
    $$ = new Node("Method", "", yylineno);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->children.push_back($5);
    $$->children.push_back($7);
  };

MethodBody:
  LBRACE RETURN Expression SC RBRACE {
    $$ = $3;
  }
  | LBRACE VarDeclarationMethod RETURN Expression SC RBRACE {
    $$ = new Node("Method body", "", yylineno);
    $$->children.push_back($2);
    $$->children.push_back($4);
  }
  | LBRACE MethodStatement RETURN Expression SC RBRACE {
    $$ = new Node("Method body", "", yylineno);
    $$->children.push_back($2);
    $$->children.push_back($4);
  };

MethodDeclarationParameter:
  Type Identifier {
    $$ = new Node("Method parameter", "", yylineno);
    $$->children.push_back($1);
    $$->children.push_back($2);
  };

MethodDeclarationParameters:
  MethodDeclarationParameter {
    $$ = new Node("Merhod parameters", "", yylineno);
    $$->children.push_back($1);
  }
  | MethodDeclarationParameters COMMA MethodDeclarationParameter {
    $$ = $1;
    $$->children.push_back($3);
  };

VarDeclarationMethod:
  VarDeclaration {
    $$ = new Node("Method variabels", "", yylineno);
    $$->children.push_back($1);
  }
  | VarDeclarationMethod VarDeclaration {
    $$ = $1;
    $$->children.push_back($2);
  };

MethodStatement:
  Statement {
    $$ = new Node("Method statement", "", yylineno);
    $$->children.push_back($1);
  }
  | MethodStatement Statement {
    $$ = $1;
    $$->children.push_back($2);
  };

Type:
    INT LBRACKET RBRACKET {
        $$ = new Node("Type", "int[]", yylineno);
    }
    | BOOL {
        $$ = new Node("Type", "bool", yylineno);
    }
    | INT {
        $$ = new Node("Type", "int", yylineno);
    }
    | Identifier {
        $$ = new Node("Type", "ID", yylineno);
        $$->children.push_back($1);
    };

Statement:
    LBRACE RBRACE {
        $$ = new Node("Empty statement", "", yylineno);
    }
    | LBRACE Statements RBRACE {
        $$ = $2;
    }
    | IF LPAR Expression RPAR Statement %prec "then" {
        $$ = new Node("If statement", "", yylineno);
        $$->children.push_back($3); // Expression
        $$->children.push_back($5); // Statement
    }
    | IF LPAR Expression RPAR Statement ELSE Statement {
        $$ = new Node("If else", "", yylineno);
        $$->children.push_back($3); // Expression
        $$->children.push_back($5); // If Statement
        $$->children.push_back($7); // Else Statement
    }
    | WHILE LPAR Expression RPAR Statement {
        $$ = new Node("While loop", "", yylineno);
        $$->children.push_back($3);
        $$->children.push_back($5);
    }
    | PRINTLN LPAR Expression RPAR SC {
        $$ = new Node("Print", "", yylineno);
        $$->children.push_back($3);
    }
    | Identifier EQASSIGN Expression SC {
        $$ = new Node("Assignment", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Identifier LBRACKET Expression RBRACKET EQASSIGN Expression SC {
        $$ = new Node("Array assign", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
        $$->children.push_back($6);
    };

Statements:
    Statement {
        $$ = $1;
    }
    | Statements Statement {
        $$ = new Node("Statements", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($2);
    };

Expression:
    Expression AND Expression {
        $$ = new Node("&& expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression OR Expression {
        $$ = new Node("|| expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression LT Expression {
        $$ = new Node("< expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression GT Expression {
        $$ = new Node("> expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression EQ Expression {
        $$ = new Node("== expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression ADDSIGN Expression {
        $$ = new Node("+ expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression SUBSIGN Expression {
        $$ = new Node("- expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression MULSIGN Expression {
        $$ = new Node("* expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression DIVSIGN Expression {
        $$= new Node("/ expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    };

parentExpression:
    Int {
        $$ = $1;
    }
    | TRUE {
        $$ = new Node("TRUE", "", yylineno);
    }
    | FALSE {
        $$ = new Node("FALSE", "", yylineno);
    }
    | THIS {
        $$ = new Node("THIS", "", yylineno);
    }
    | Identifier {
        $$ = $1;
    }
    | parentExpression DOT LENGTH {
        $$ = new Node("Expression.Length", "", yylineno);
        $$->children.push_back($1);
    }
    | parentExpression LBRACKET Expression RBRACKET {
        $$ = new Node("Array access", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | parentExpression DOT Identifier LPAR commaExpression RPAR {
        $$ = new Node("Method call", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
        $$->children.push_back($5);
    }
    | parentExpression DOT Identifier LPAR RPAR {
        $$ = new Node("Method call", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | NEW INT LBRACKET Expression RBRACKET {
        $$ = new Node("new int[] expression", "", yylineno);
        $$->children.push_back($4);
    }
    | NEW Identifier LPAR RPAR {
        $$ = new Node("Object Instatntaion", "", yylineno);
        $$->children.push_back($2);
    }
    | LPAR Expression RPAR {
        $$ = new Node("(Expression)", "", yylineno);
        $$->children.push_back($2);
    }
    | NOT Expression {
        $$ = new Node("! expression", "", yylineno);
        $$->children.push_back($2);
    };;

commaExpression:
    Expression {
        $$ = $1;
    }
    | commaExpression COMMA Expression {
        $$ = new Node("Parameter", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    };

Identifier:
  ID {
  $$ = new Node("ID", $1, yylineno);
  };

Int:
  INT_LITERAL{
  $$ = new Node("Int", $1, yylineno);
  }
  | SUBSIGN INT_LITERAL {
  $$ = new Node("Int", "-"+ $2, yylineno);
  };
