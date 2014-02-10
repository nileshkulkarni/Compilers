
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
%union 
{
	int integer_value;
	float float_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
    Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
    Expression_Ast::OperatorType op;
    Data_Type data_type;
};


%token <integer_value> INTEGER_NUMBER
%token <integer_value> BASIC_BLOCK
%token <float_value> FLOAT_NUMBER
%token <string_value> NAME
%token RETURN INTEGER FLOAT DOUBLE 
%token IF ELSE GOTO
%token ASSIGN_OP  
%left NE EQ
%left LT LE GT GE 
%left '+' '-'
%left '*' '/'

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
	'{' declaration_statement_list    	basic_block_list '}'
|
	'{' basic_block_list '}'
;

declaration_statement_list:
	declaration_statement
|
	declaration_statement_list declaration_statement
;

declaration_statement:
	FLOAT NAME ';'
 |
	INTEGER NAME ';'
;

basic_block_list:
	basic_block_list basic_block
|
	basic_block
	
;

basic_block:
	BASIC_BLOCK ':' executable_statement_list
;

executable_statement_list:
	assignment_statement_list
|
	assignment_statement_list RETURN ';'
|   
    assignment_statement_list goto_statement
| 
   assignment_statement_list if_else_statement;

assignment_statement_list:   
    |
	assignment_statement_list assignment_statement
;


if_else_statement: IF '(' expression ')' goto_statement ELSE goto_statement
;

            
goto_statement: GOTO BASIC_BLOCK ';'  
; 
              
atomic_expression: variable
     |
     constant
;

unary_expression: '-' atomic_expression
                |
                '-' unary_expression
;



arithmetic_expression: atomic_expression
        |
        Big_Expressin
;


type_casted_expression: arithmetic_expression
        | '(' DATA_TYPE ')' '(' Big_Expression ')'
        | '(' DATA_TYPE ')' atomic_expression
;
					 


Big_Expression : unary_expression
    |type_casted_expression '+' type_casted_expression           
    |type_casted_expression '-' type_casted_expression
    |type_casted_expression '*' type_casted_expression
    |type_casted_expression '/' type_casted_expression    
    |type_casted_expression LE type_casted_expression
    |type_casted_expression GE type_casted_expression
    |type_casted_expression GT type_casted_expression           
    |type_casted_expression LT type_casted_expression		    
    |type_casted_expression EQ type_casted_expression
    |type_casted_expression NE type_casted_expression
;


 
expression : type_casted_expression
;


assignment_statement:
	variable ASSIGN_OP expression ';'
	
;

DATA_TYPE : FLOAT
          |DOUBLE
          |INTEGER
;

variable:
	NAME
	
;

constant:
	INTEGER_NUMBER
	
    |
	FLOAT_NUMBER
	;
