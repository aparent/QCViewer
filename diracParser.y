%{
  #include <stdio.h>
  #include <string.h>
  #include <iostream>
  #include <string>
  #include "parseNode.h"
  #define YYSTYPE parseNode*
  using namespace std;
  string printTree(parseNode *node);
  void yyerror(const char *s);
  int yylex();
  int yy_scan_string(const char*);
  parseNode *final;
%}
%error-verbose
%defines "diracParser.h"
%start input
%token CNUM NUM KET PLUS MINUS TIMES DIV SQRT /* CNUM is for complex numbers  */
%left low
%left MINUS PLUS
%left TIMES
%left DIV
%left SQRT
%right high

%%
input:       /*empty*/
             | exp                      { final = $1; }
     ;
     exp:      subex                    { $$ = $1;    }
              | exp PLUS exp              {
                                          $2->left = $1; $2->right=$3;
                                           $$ = $2;
                                         }
              | exp MINUS exp            {
                                          $2->left = $1; $2->right=$3;
                                           $$ = $2;
                                         }
             | exp TIMES exp             {
                                           $2->left = $1; $2->right=$3;
                                           $$ = $2;
                                         }
     ;
     subex: term
             | subex subex  %prec low     {
                                          parseNode * val = new parseNode;
                                          val->value = "*"; val->left = $1; val->right=$2;
                                          val->type = TIMES;
                                           $$ = val;
                                          }
             | subex DIV NUM               {
                                           $2->left = $1; $2->right=$3;
                                           $$ = $2;
                                         }
             | subex DIV '(' exp ')'      {
                                           $2->left = $1; $2->right=$4;
                                           $$ = $2;
                                         }
             | subex DIV SQRT '(' exp ')' {
                                           $2->left = $1; $2->right=$3;
                                          $3->right = $5;
                                           $$ = $2;
                                         }
            |  SQRT '(' exp ')'         {
                                           $1->right = $3;
                                          $$ = $1;
                                         }
             | '(' exp ')'              { $$ = $2; }
     ;

     term:     KET                        { $$ = $1; }
              |NUM                       { $$ = $1; }
              |CNUM                      { $$ = $1; }
     ;

%%


parseNode *parseDirac(string input){
  char *in = (char*)malloc(input.length() + 1);
  strcpy(in,input.c_str());
  yy_scan_string(in);
  yyparse ();
  return final;
}

void yyerror (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}
