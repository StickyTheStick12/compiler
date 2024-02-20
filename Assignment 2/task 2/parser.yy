%skeleton "lalr1.cc"
%defines
%define parse.error verbose
%define api.value.type variant
%define api.token.constructor

%code requires{
  #include <string>
  #include <stdio.h>
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
%type <Node*> Goal MainClass StatementList ClassDeclarationList ClassDeclaration ClassBody VarDeclarationClassList VarDeclaration MethodDeclarationList MethodDeclaration MethodBody MethodDeclarationParameter MethodList Method MethodDeclarationParameters Type Statement Statements Expression PrimaryExpression commaExpression Identifier Int

%%

Goal:
    MainClass END {
        $$ = new Node("Program", "", yylineno);
        root = $$;
        $$->children.push_back($1);
    }
    | MainClass ClassDeclarationList END {
        $$ = $2;
        $$->children.push_front($1);
        root = $$;
    };

MainClass:
    PUBLIC CLASS Identifier LBRACE PUBLIC STATIC VOID MAIN LPAR STRING LBRACKET RBRACKET Identifier RPAR LBRACE StatementList RBRACE RBRACE {
    $$ = new Node("Main Class", $3->value, yylineno);
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
    $$ = new Node("Program", "", yylineno);
    $$->children.push_back($1);
  }
  | ClassDeclarationList ClassDeclaration {
    $$ = $1;
    $$->children.push_back($2);
  };

ClassDeclaration:
    CLASS Identifier ClassBody {
        $$ = new Node("Class", $2->value, $2->lineno);
        $$->children.push_back($3); // VarDeclaration
    };

ClassBody:
    LBRACE RBRACE {
        $$ = new Node("No class body", "", yylineno);
    }
    | LBRACE VarDeclarationClassList RBRACE {
        $$ = $2;
    }
    | LBRACE MethodDeclarationList RBRACE {
        $$ = $2;
    }
    | LBRACE VarDeclarationClassList MethodDeclarationList RBRACE {
        $$ = new Node("Class body", "", yylineno);
        $$->children.push_back($2);
        $$->children.push_back($3);
    };

MethodDeclarationList:
 MethodDeclaration {
    $$ = new Node("MethodDecList", "", yylineno);
    $$->children.push_back($1);
  }
  | MethodDeclarationList MethodDeclaration {
    $$ = $1;
    $$->children.push_back($2);
  };

VarDeclarationClassList:
    VarDeclaration {
        $$ = new Node("Class variables", "", yylineno);
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
    $$ = new Node("Method", $3->value, $2->lineno);
    $$->children.push_back($2);
    $$->children.push_back($6);
  }
  | PUBLIC Type Identifier LPAR MethodDeclarationParameters RPAR MethodBody {
    $$ = new Node("Method", $3->value, $2->lineno);
    $$->children.push_back($2);
    $$->children.push_back($5);
    $$->children.push_back($7);
  };

MethodBody:
  LBRACE RETURN Expression SC RBRACE {
    $$ = $3;
  }
  | LBRACE MethodList RETURN Expression SC RBRACE {
    $$ = new Node("Method body", "", yylineno);
    $$->children.push_back($2);
    $$->children.push_back($4);
  };

MethodDeclarationParameter:
  Type Identifier {
    $$ = new Node("Variable", "", yylineno);
    $$->children.push_back($1);
    $$->children.push_back($2);
  };

MethodDeclarationParameters:
  MethodDeclarationParameter {
    $$ = new Node("Method parameters", "", yylineno);
    $$->children.push_back($1);
  }
  | MethodDeclarationParameters COMMA MethodDeclarationParameter {
    $$ = $1;
    $$->children.push_back($3);
  };

MethodList:
    Method {
        $$ = new Node("MethodList", "", yylineno);
        $$->children.push_back($1);
    }
    | MethodList Method {
        $$ = $1;
        $$->children.push_back($2);
    };

Method:
    VarDeclaration {
        $$ = new Node("Method variable", "", yylineno);
        $$->children.push_back($1);
        $$ = $1;
    }
    | Statement {
        $$ = new Node("Method statement", "", yylineno);
        $$->children.push_back($1);
        $$ = $1;
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
        $$ = new Node("Type", $1->value, yylineno);
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
        $$ = new Node("Array assignment", "", yylineno);
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
        $$ = new Node("and expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression OR Expression {
        $$ = new Node("or expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression LT Expression {
        $$ = new Node("LT expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression GT Expression {
        $$ = new Node("GT expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression EQ Expression {
        $$ = new Node("EQ expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression ADDSIGN Expression {
        $$ = new Node("Add expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression SUBSIGN Expression {
        $$ = new Node("Sub expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression MULSIGN Expression {
        $$ = new Node("Mul expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | Expression DIVSIGN Expression {
        $$= new Node("Div expression", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | NOT Expression {
        $$ = new Node("Not expression", "", yylineno);
        $$->children.push_back($2);
    }
    | PrimaryExpression {
        $$ = $1;
    };

PrimaryExpression:
    Int {
        $$ = $1;
    }
    | Identifier {
        $$ = $1;
    }
    | TRUE {
        $$ = new Node("TRUE", "", yylineno);
    }
    | FALSE {
        $$ = new Node("FALSE", "", yylineno);
    }
    | THIS {
        $$ = new Node("THIS", "this", yylineno);
    }
    | PrimaryExpression LBRACKET Expression RBRACKET {
        $$ = new Node("Array access", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | PrimaryExpression DOT LENGTH {
        $$ = new Node("Expression.Length", "", yylineno);
        $$->children.push_back($1);
    }
    | PrimaryExpression DOT Identifier LPAR commaExpression RPAR {
        $$ = new Node("Method call", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
        $$->children.push_back($5);
    }
    | PrimaryExpression DOT Identifier LPAR RPAR {
        $$ = new Node("Method call", "", yylineno);
        $$->children.push_back($1);
        $$->children.push_back($3);
    }
    | NEW INT LBRACKET Expression RBRACKET {
        $$ = new Node("New int[] expression", "", yylineno);
        $$->children.push_back($4);
    }
    | NEW Identifier LPAR RPAR {
        $$ = new Node("Object instantiation", "", yylineno);
        $$->children.push_back($2);
    }
    | LPAR Expression RPAR {
        $$ = new Node("(Expression)", "", yylineno);
        $$->children.push_back($2);
    };


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
  $$ = new Node("Int", "-" + $2, yylineno);
  };
