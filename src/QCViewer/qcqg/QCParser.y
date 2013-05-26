
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

Authors: Alex Parent, Jacob Parker, Marc Burns
---------------------------------------------------------------------*/

%{
  #include <stdio.h>
  #include <string.h>
	#include <cstdlib>
	#include <fstream>
  #include <string>
  #include <memory>
  #include <vector>
  #include <iostream>
  #include "utility.h"
  #include "circuit.h"
  #include "QCParserUtils.h"
  void QC_error(const char *s);
  int QC_lex();
  int QC__scan_string(const char*);
  std::shared_ptr<Circuit> circuit;
  std::shared_ptr<Circuit> curr_circ;
  std::vector<std::string> error_log;
  #define CHECK_NAMES(names,id) if(!check_names(curr_circ,names,error_log,id)){circuit=std::shared_ptr<Circuit>();return -1;}
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
%token VARS INPUTS OUTPUTS CONSTANTS OUTLABELS START END PI NEWLINE NUM WORD APOS LBRAC RBRAC EXPON NEG COLON PLUS DIV BAR

%type <string> WORD NUM id
%type <names> names nums
%type <fnum> float


%%
input:	/*empty*/
		    | input NEWLINE
     		| input header
		    | input START WORD LBRAC names RBRAC NEWLINE
			    {
            curr_circ = std::shared_ptr<Circuit>(new Circuit());
			      curr_circ->setName($3);
			      add_lines(curr_circ,$5);
          }
			    gates
          {
            circuit->subcircuits[sToUpper($3)]= curr_circ;
          }
          END WORD
		    | input START NEWLINE
          {
            curr_circ = circuit;
          }
          gates END
		    | error
          {
            circuit = std::shared_ptr<Circuit>();
            return -1;
          }
;

header: VARS names NEWLINE
          {
            add_lines(circuit,$2);
          }
     		| INPUTS names NEWLINE
          {
           CHECK_NAMES($2,"inputs");
           add_inputs(circuit,$2);
          }
     		| OUTPUTS names NEWLINE
          {
            CHECK_NAMES($2,"outputs");
            add_outputs(circuit,$2);
          }
     		| CONSTANTS nums NEWLINE
          {
            add_constants(circuit,$2);
          }
     		| OUTLABELS names NEWLINE
          {
            add_outlabels(circuit,$2);
          }

gates:  /*empty*/
        | gates WORD names NEWLINE
          {
            CHECK_NAMES($3,$2);
            add_gate(curr_circ,$2,$3,1,error_log);
          }
				/*gate with colon seperated controls/targets*/
        | gates WORD names BAR names NEWLINE
          {
            CHECK_NAMES($3,$2);
            CHECK_NAMES($5,$2);
            add_gate(curr_circ,$2,$3,$5,1,error_log);
          }
				/*Rotation gate by float*/
        | gates WORD LBRAC float RBRAC names NEWLINE
          {
            CHECK_NAMES($6,$2);
            addRGate(curr_circ,$2,$6,1,$4);
          }
				/*rotation gate by fraction*/
        | gates WORD LBRAC NUM PI DIV NUM RBRAC names NEWLINE
          {
            CHECK_NAMES($9,$2);
            addFracRGate(curr_circ,$2,$9,1,atoi($4),atoi($7));
          }
				/* gate which is looped */
        | gates WORD EXPON NUM names NEWLINE
          {
            CHECK_NAMES($5,$2);
            add_gate(curr_circ,$2,$5,atoi($4),error_log);
          }
				/*Rotation gate by float shich is looped*/
        | gates WORD LBRAC float RBRAC EXPON NUM names NEWLINE
          {
            CHECK_NAMES($8,$2);
            addRGate(curr_circ,$2,$8,atoi($7),$4);
          }
				/*rotation gate by fraction which is looped*/
        | gates WORD LBRAC NUM PI DIV NUM RBRAC EXPON NUM names NEWLINE
          {
            CHECK_NAMES($11,$2);
            addFracRGate(curr_circ,$2,$11,atoi($10),atoi($4),atoi($7));
          }
				/*A set of one bit gates to be applied to a single qubit*/
        | gates id COLON WORD NEWLINE
          {
            add_one_bit_gates(curr_circ,$2,$4);
          }
        | gates NEWLINE
          {
            insert_break(curr_circ);
          }
;

names:/*empty*/ {$$ = NULL;}
			|  id names
        {
          $$ = new name_node($1,$2);
        }
			|  id APOS names
        {
          $$ = new name_node($1,$3,true);
        }

id:   WORD
				{
					$$ = $1;
				}
		| NUM
				{
					$$ = $1;
				}

nums:/*empty*/ {$$ = NULL;}
			|  NUM nums
        {
          $$ = new name_node($1,$2);
        }

float: NUM
        {
          $$=atof($1);
        }
			| NEG NUM
        {
          $$=-atof($2);
        }
;

%%

#include "QCParserUtils.h"
std::shared_ptr<Circuit> parseCircuit(std::string text,std::vector<std::string>& error_log_r )
{
	error_log.clear();
	circuit = std::shared_ptr<Circuit>(new Circuit());
	curr_circ = circuit;
	circuit->setName("Main");
  char *in = (char*)malloc(text.length() + 1);
  strcpy(in,text.c_str());
  QC__scan_string(in);
  QC_parse ();
	if (!circuit){
	  error_log_r = error_log;
		return circuit;
	}
	link_subcircs(circuit);
	cleanup_bad_gates(circuit,error_log);
	error_log_r = error_log;
  return circuit;
}

void QC_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}
