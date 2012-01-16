
/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011  Alex Parent and The University of Waterloo, 
Institute for Quantum Computing, Quantum Circuits Group

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

See also 'ADDITIONAL TERMS' at the end of the included LICENSE file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

QCViewer is a trademark of the of the The University of Waterloo, 
Institute for Quantum Computing, Quantum Circuits Group

Authors: Alex Parent, Jakub Parker
---------------------------------------------------------------------*/

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
	double fnum;
	name_node *names;
}
%name-prefix "QC_"
%error-verbose
%debug
%verbose
%defines "QCParser.h"
%output "QCParser.cpp"
%start input
%token VARS INPUTS OUTPUTS CONSTANTS OUTLABELS START END NEWLINE NUM WORD APOS LBRAC RBRAC EXPON NEG

%type <string> WORD NUM
%type <names> names nums
%type <fnum> float

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
				| error {circuit = NULL;}
;
gates:  /*empty*/	
				| WORD names NEWLINE {add_gate(curr_circ,$1,$2,1,subcircuits);} gates 
				| WORD LBRAC float RBRAC names {add_R_gate(curr_circ,$1,$5,1,$3);} NEWLINE gates
				| WORD EXPON NUM names NEWLINE {add_gate(curr_circ,$1,$4,atoi($3),subcircuits);} gates
				| WORD LBRAC float RBRAC EXPON NUM names NEWLINE {add_R_gate(curr_circ,$1,$7,atoi($6),$3);} gates
				| NEWLINE gates
;
names:/*empty*/ {$$ = NULL;}
			|  WORD names {$$ = new name_node($1,$2);} 
			|  NUM names {$$ = new name_node($1,$2);}
			|  WORD APOS names {$$ = new name_node($1,$3,true);}
			|  NUM APOS names {$$ = new name_node($1,$3,true); }

nums:/*empty*/ {$$ = NULL;}
			|  NUM nums {$$ = new name_node($1,$2);}

float: NUM {$$=atof($1);}
			| NEG NUM {$$=-atof($2)}
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
