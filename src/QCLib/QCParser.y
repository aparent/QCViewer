%{
  #include <stdio.h>
  #include <string.h>
	#include <fstream>
  #include <string>
  #include "circuit.h"
  void QC_error(const char *s);
  int QC_lex();
  int QC__scan_string(const char*);
  Circuit *circuit = new Circuit();
%}
%code requires{
  #include "QCParserUtils.h"
}
%union {
	char *string;
	int token;
	name_node *names;
}
%name-prefix "QC_"
%error-verbose
%debug
%verbose
%defines "QCParser.h"
%output "QCParser.cpp"
%start input
%token VARS INPUTS OUTPUTS CONSTANTS OUTLABELS START END NEWLINE NUM WORD APOS LBRAC RBRAC EXPON
%type <string> WORD NUM
%type <names> names

%%
input:	/*empty*/
     		| VARS names NEWLINE input
     		| INPUTS names NEWLINE input
     		| OUTPUTS names NEWLINE input
     		| CONSTANTS names NEWLINE input
     		| OUTLABELS names NEWLINE input
				| NEWLINE input
				| START WORD LBRAC names RBRAC NEWLINE gates END input
				| START WORD NEWLINE gates END WORD input
				| START NEWLINE gates END input
;
gates:  /*empty*/	
				| WORD names NEWLINE gates
				| WORD LBRAC names RBRAC names  NEWLINE gates
				| WORD EXPON NUM names NEWLINE gates
				| WORD LBRAC names RBRAC EXPON NUM names NEWLINE gates
				| NEWLINE gates
;
names:/*empty*/ {$$ = NULL;}
			|  WORD names {$$ = new name_node($1,$2);} 
			|  NUM names {$$ = new name_node($1,$2);}
			|  WORD APOS names {$$ = new name_node($1,$3);}
			|  NUM APOS names {$$ = new name_node($1,$3); }
;
%%

Circuit *parseCircuit(string filename){
	string input,line;
	ifstream myfile(filename.c_str());
  if (myfile.is_open()) {
      while ( myfile.good() ) {
          getline (myfile,line);
          input += line + "\n";
      }
  }
  char *in = (char*)malloc(input.length() + 1);
  strcpy(in,input.c_str());
  QC__scan_string(in);
  QC_parse ();
  return NULL;
}

void QC_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}
