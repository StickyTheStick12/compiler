#include "stdio.h"

extern int yylex();
extern FILE* yyin;


int main(int argc, char **argv)
{
  //Reads from file if a file name is passed as an argument. Otherwise, reads from stdin.
  if(argc > 1) {
    if(!(yyin = fopen(argv[1], "r"))) {
      perror(argv[1]);
      return 1;
    }
  }

  if (!yylex()) {
      printf("Done!\n");
  }
  return 0;
}
