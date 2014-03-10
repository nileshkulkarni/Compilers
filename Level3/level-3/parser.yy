
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
%token RETURN INTEGER FLOAT DOUBLE VOID
%token IF ELSE GOTO
%token ASSIGN_OP  

%left NE EQ
%left LT LE GT GE 
%left '+' '-'
%left '*' '/'


%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
%type <ast> goto_statement
%type <ast> if_else_statement
%type <ast> function_call
%type <ast> variable
%type <ast> atomic_expression
%type <ast> unary_expression
%type <ast> expression
%type <ast> constant
%type <data_type> DATA_TYPE 
%start program


%%
program:

	declaration_statement_list  
	{
		program_object.set_global_table(*$1);
		return_statement_used_flag = false;				
        // No return statement in the current procedure till now
	
    }
    procedure_declaration_list 	
    procedure_list
        

|
	declaration_statement_list     
	{
		program_object.set_global_table(*$1);
		return_statement_used_flag = false;				
        // No return statement in the current procedure till now
	
    } 
    procedure_list
|

	procedure_declaration_list  procedure_list
|
    procedure_list
;


procedure_declaration_list :procedure_declaration_list procedure_declaration 
							|	
							procedure_declaration
;

procedure_declaration :
                      DATA_TYPE NAME '(' parameter_list ')' ';'
                      {
                          if(program_object.variable_in_symbol_list_check(*$2)){
                                report_error("Variable name matched procedure", get_line_number());
                          
                          }

                          program_object.variable_in_proc_map_check(*$2);
                        
                          current_procedure= new Procedure($1,*$2);
                          current_procedure->set_local_list(*$4)
                          program_object.set_procedure_map(*current_procedure);
                          delete $2;

                      }
                      |
					  VOID NAME '(' parameter_list ')' ';'
                      {
                          if(program_object.variable_in_symbol_list_check(*$2)){
                                report_error("Variable name matched procedure", get_line_number());
                          
                          }

                          program_object.variable_in_proc_map_check(*$2);
                        
                          current_procedure= new Procedure($1,*$2);
                          current_procedure->set_local_list(*$4)
                          delete $2;

                      }
;


procedure_list: procedure_list procedure
              |
              procedure
;

procedure : procedure_name procedure_body
;




procedure_name:
	NAME '(' parameter_list ')'
	{
        
         Procedure *P = program_object->get_procedure(*$1);
         if((*$1) == "main"){
            if(P !=NULL){
                report_error("Main function declared twice",get_line_number());
            }
            
            Procedure *main = new Procedure(Data_Type::void_data_type,"main");
            program_object->set_procedure_map(*main);
            P = main;
        }
        else if(P == NULL){
                report_error("Procedure correspoding to the name not found\n");
        }


         if(P->check_parameter_list(*$3)){
            report_error("Parameters in definitions and declartion do not match());
         }
            
        current_procedure = P;
        return_statement_used_f_procedure_map(*current_procedure);;

procedure_body:
	'{' 
	declaration_statement_list 
    {
		current_procedure->append_local_list(*$2);
        delete $2;
    }
	
    basic_block_list
	{
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}

		current_procedure->set_basic_block_list(*$4);
        
        //int bbNotExist = current_procedure->check_for_undefined_blocks(bbNo,gotoNo);
		delete $4;
        	
    }
    '}'
|
	'{' basic_block_list '}'
	{
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			report_error("Atleast 1 basic block should have a return statement", line);
		}

		current_procedure->set_basic_block_list(*$4);
        
        //int bbNotExist = current_procedure->check_for_undefined_blocks(bbNo,gotoNo);
		delete $4;
    }
;

parameter_list : one_or_more_parameter_list
               {
                    $$ = $1

                }
              |
              {
                $$  = new Symbol_Table();

            }

; 

 
one_or_more_parameter_list :
				one_or_more_parameter_list ',' parameter
                {
                    if($1 == NULL){
                        report_internal_error("Parameter list is null ") 
                    }
                    
                    $1->variable_in_symbol_list_check($3->get_variable_name());
                    if(current_procedure->get_proc_name() == $3->get_variable_name()){
                        report_error("Variable name cannot be same as the name of the function",get_line_number()); 
                    }

                    $1->push_symbol($3);
                    $$ = $1;
                }   
				|
				parameter{
                    $$  = new Symbol_Table();
                    $$->push_symbol($1);
                }
;


parameter : DATA_TYPE NAME{
            $$ = new Symbol_Table_Entry(*$2,$1);             
            delete $2;
    }
;


argument_list : one_or_more_argument_list
              |
; 
              

one_or_more_argument_list :one_or_more_argument_list ',' argument
             {
                if($1 ==NULL){
                    report_internal_error(" argument list is null");
                }
                $$->push_back($3);
			  }
              |
              argument
              { 
			        $$ = new list<Ast *>;
                    $$->push_back($1);
              }
;

argument : expression
         {  
            $$ =$1;
         }   
;


declaration_statement_list:
	declaration_statement
	{
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		$$ = new Symbol_Table();
		$$->push_symbol($1);
    }
	
|
	declaration_statement_list declaration_statement
	{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		int line = get_line_number();
		program_object.variable_in_proc_map_check($2->get_variable_name(), line);

		string var_name = $2->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($2);
    }
	
;

declaration_statement:
	DATA_TYPE NAME ';'
	{
		$$ = new Symbol_Table_Entry(*$2, $1);
		delete $2;
	
    }
	
;

basic_block_list:
	basic_block_list basic_block
	{
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->push_back($2);
    }
	
|
	basic_block
	{
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}
         
		$$ = new list<Basic_Block *>;
		$$->push_back($1);
        
	
    }
	
	
	
	
;

basic_block:
	BASIC_BLOCK ':' executable_statement_list
	{
        
		if ($3 != NULL)
			$$ = new Basic_Block($1, *$3);
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);

		}

        bbNo.insert($1);

	
    
    }
;

executable_statement_list:
	assignment_statement_list
	{
		$$ = $1;;
	
    }
|
	assignment_statement_list return_statement
	{
		Ast * ret = new Return_Ast();

		return_statement_used_flag = true;	
        // Current procedure has an occurrence of return statement

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back(ret);
	
    }
|   
    assignment_statement_list goto_statement  
    {

		// Current procedure has an occurrence of return statement

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
    
    
    } 
| 
   assignment_statement_list if_else_statement
   {
		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back($2);
   
   }
    
;

assignment_statement_list:   {
			$$ = NULL;
    
    }

    |
	assignment_statement_list assignment_statement{
        if ($1 == NULL)
            $$ = new list<Ast *>;

        else
            $$ = $1;

        $$->push_back($2);
    }

;

assignment_statement:
	variable ASSIGN_OP expression ';'
	{
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
    }
	
    |
   function_call ';' 

;

if_else_statement: IF '(' expression ')' goto_statement ELSE goto_statement{
                  $$ =new IfElse_Ast($3,(Goto_Ast*)$5,(Goto_Ast*)$7);   
                 }


;


return_statement: RETURN ';'
				|
				   RETURN expression ';'
;

            
goto_statement: GOTO BASIC_BLOCK ';'  {
                $$ =  new Goto_Ast($2);
                gotoNo.insert($2);
              
              }

; 
              
atomic_expression: variable{
                 $$ = $1;
     
     }

     |
     constant{
            $$ =$1;
     
     }

     |
     '(' expression ')'{
            $$ = $2;

        
        }
    |
    function_call
;

unary_expression: atomic_expression{
                    $$ =$1;
				
                }

                |
                '-' unary_expression{
                    Ast*  unaryExp = new UnaryExpression_Ast($2,Expression_Ast::OperatorType::UMINUS);
                    $$ = unaryExp;
                
                }

;


function_call : NAME '(' argument_list ')' {
                
                Ast* func = new Function_call_Ast($1,$3);
                $$ = func;
            }
;

expression: unary_expression{
            $$ = $1;
          
          }

    | expression LE expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::LE ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | expression GE expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::GE ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }
 
    | expression LT expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::LT ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | expression GT expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::GT ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | expression NE expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::NE ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | expression EQ expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::EQ ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | expression '+' expression{         
           assert($3 != NULL);
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::PLUS ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | expression '-' expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::MINUS ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
            }
    | expression '*' expression{
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::MULT ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | expression '/' expression{   
           Ast* exp = new Expression_Ast($1,$3,Expression_Ast::OperatorType::DIV ); 
           $$ = exp;
           int line = get_line_number();
           $$->check_ast(line);
           
           }

    | '(' DATA_TYPE ')' atomic_expression{
            Ast* exp = new Expression_Ast($4,$2); 
            $$ = exp;
            
        
        }

;


DATA_TYPE : FLOAT{
            Data_Type data_type = float_data_type;             
            $$ = data_type;
        
        }

        |DOUBLE{
            Data_Type data_type = double_data_type;             
            $$ = data_type;

        
        }

        |INTEGER{
            Data_Type data_type = int_data_type;             
            $$ = data_type;

        
        }

;

variable:
	NAME
    {
		Symbol_Table_Entry var_table_entry;

		if (current_procedure->variable_in_symbol_list_check(*$1)){
			 var_table_entry = current_procedure->get_symbol_table_entry(*$1);
		}

		else if (program_object.variable_in_symbol_list_check(*$1)){
			var_table_entry = program_object.get_symbol_table_entry(*$1);
		}
		else
		{
			int line = get_line_number();
			report_error("Variable has not been declared", line);
		}

		$$ = new Name_Ast(*$1, var_table_entry);

		delete $1;
	
    }
	
			
;

constant:
	INTEGER_NUMBER
	{
		$$ = new Number_Ast<int>($1, int_data_type);
		//cout<<"$1 : "<<$1<<endl;

	
    }
	
	|
	FLOAT_NUMBER
	{
		$$ = new Number_Ast<int>($1, int_data_type);
		//cout<<"$1 : "<<$1<<endl;

	
    }
	
	;
