
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc
%debug
%union 
{
	int integer_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
};

%token <integer_value> INTEGER_NUMBER
%token <string_value> NAME
%token RETURN INTEGER 
%token IF ELSE GOTO

%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
%type <ast> goto_statement
%type <ast> if_else_statement
%type <ast> variable
%type <ast> constant

%start program

%%

program:
	declaration_statement_list procedure_name
	procedure_body
|
	procedure_name
	procedure_body
;

procedure_name:
	NAME '(' ')'
;

procedure_body:
	'{' declaration_statement_list
    	basic_block_list '}'
|
	'{' basic_block_list '}'
;

declaration_statement_list:
	declaration_statement
|
	declaration_statement_list declaration_statement
;

declaration_statement:
	INTEGER NAME ';'
;

basic_block_list:
	basic_block_list basic_block
|
	basic_block
	
;

basic_block:
	'<' NAME INTEGER_NUMBER '>' ':' executable_statement_list
;

executable_statement_list:
	assignment_statement_list
|
	assignment_statement_list RETURN ';'
|   
    assignment_statement_list goto_statement 
| 
   assignment_statement_list if_else_statement
;

assignment_statement_list:
    |
	assignment_statement_list assignment_statement
;


if_else_statement: IF '(' expression ')' goto_statement ELSE goto_statement{
                  //  std::cout<<"Came to if \n";
                 };

            
goto_statement: GOTO '<'NAME INTEGER_NUMBER'>' ';' {
              //std::cout<<"Came to goto statement\n";
              }
              ; 
              

relational_op : '<' | '>' | '<''=' | '>''=' | '=''=' | '!''=';

expression:
 expression relational_op  expression
| variable 
| constant
;

assignment_statement:
	variable '=' variable ';'
|
	variable '=' constant ';'
;

variable:
	NAME
;

constant:
	INTEGER_NUMBER
;
