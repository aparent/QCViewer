%{
  #include <stdio.h>
  #include <string.h>
	#include <cstdlib>
	#include <fstream>
  #include <string>
  #include <iostream>
  #include "circuit.h"
  void QC_error(const char *s);
  int QC_lex();
  int QC__scan_string(const char*);
  Circuit *circuit;
  Circuit *curr_circ;
	map<string,Circuit> subcircuits;
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
%type <names> names nums

%%
input:	/*empty*/
     		| VARS names NEWLINE {add_lines(circuit,$2);} input
     		| INPUTS names NEWLINE {add_inputs(circuit,$2);} input
     		| OUTPUTS names NEWLINE {add_outputs(circuit,$2);} input
     		| CONSTANTS nums NEWLINE {add_constants(circuit,$2);} input
     		| OUTLABELS names NEWLINE {add_outlabels(circuit,$2);} input
				| NEWLINE input
				| START WORD LBRAC names RBRAC NEWLINE 
					{ curr_circ = new Circuit();  
						curr_circ->name = $2;
						add_lines(curr_circ,$4); }
					gates {subcircuits[$2]= *curr_circ;} END WORD input
				| START NEWLINE {curr_circ = circuit;} gates END input
;
gates:  /*empty*/	
				| WORD names NEWLINE {add_gate(curr_circ,$1,$2,1,subcircuits);} gates 
				| WORD LBRAC NUM RBRAC names {add_R_gate(curr_circ,$1,$5,1,atof($3));} NEWLINE gates
				| WORD EXPON NUM names NEWLINE {add_gate(curr_circ,$1,$4,atoi($3),subcircuits);} gates
				| WORD LBRAC NUM RBRAC EXPON NUM names NEWLINE {add_R_gate(curr_circ,$1,$7,atoi($6),atof($3));} gates
				| NEWLINE gates
;
names:/*empty*/ {$$ = NULL;}
			|  WORD names {$$ = new name_node($1,$2);} 
			|  NUM names {$$ = new name_node($1,$2);}
			|  WORD APOS names {$$ = new name_node($1,$3);}
			|  NUM APOS names {$$ = new name_node($1,$3); }

nums:/*empty*/ {$$ = NULL;}
			|  NUM nums {$$ = new name_node($1,$2);}
;
%%

Circuit *parseCircuit(string filename){
	circuit = new Circuit();
	string input,line;
	ifstream myfile(filename.c_str());
  if (myfile.is_open()) {
      while ( myfile.good() ) {
          getline (myfile,line);
          input += line + "\n";
      }
			//-----------testcode--------------
			circuit->name="Test";
			//---------------------------------
  		char *in = (char*)malloc(input.length() + 1);
  		strcpy(in,input.c_str());
  		QC__scan_string(in);
  		QC_parse ();
  }
	else{ 
		delete circuit;
		circuit = NULL;
	}
  return circuit;
}

void QC_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}
