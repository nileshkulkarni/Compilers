
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

#include<iostream>
#include<fstream>
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"local-environment.hh"
#include"icode.hh"
#include"reg-alloc.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"
#include <cassert>

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	stringstream msg;
	msg << "No print_value() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	stringstream msg;
	msg << "No get_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	stringstream msg;
	msg << "No set_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;

	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}


Data_Type Expression_Ast::get_data_type()
{
	return node_data_type;
}


bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}
	
	cout<<lhs->get_data_type()<<" : "<<rhs->get_data_type()<<endl;

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

void Assignment_Ast::print(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(result.is_variable_defined(), "Variable should be defined to be on rhs of Assignment_Ast", lineno);
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result
	print(file_buffer);
	lhs->print_value(eval_env, file_buffer);
	return result;
}

Code_For_Ast & Assignment_Ast::compile()
{
	/* 
		An assignment x = y where y is a variable is 
		compiled as a combination of load and store statements:
		(load) R <- y 
		(store) x <- R
		If y is a constant, the statement is compiled as:
		(imm_Load) R <- y 
		(store) x <- R
		where imm_Load denotes the load immediate operation.
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();
	load_register->set_used_for_expr_result(true);
	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

//!!

	load_register->set_used_for_expr_result(false);
	return *assign_stmt;
}

Code_For_Ast & Assignment_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast load_stmt;
	
	
//!!?????	
	if((typeid(*rhs) == typeid(Number_Ast<int>)) ||
	   (typeid(*rhs) == typeid(Number_Ast<float>)) ||
	   (typeid(*rhs) == typeid(Number_Ast<double>)) ||
	    (typeid(*rhs) == typeid(Name_Ast))){
		lra.optimize_lra(mc_2m, lhs, rhs);
	}
	
	
	load_stmt = rhs->compile_and_optimize_ast(lra);
	
	Register_Descriptor * result_register = load_stmt.get_reg();
	Code_For_Ast store_stmt = lhs->create_store_stmt(result_register);
	
	list<Icode_Stmt *> ic_list;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);
		
	
	Symbol_Table_Entry *lhs_symbol_entry = &(lhs->get_symbol_entry());
	Register_Descriptor *old_register = lhs_symbol_entry->get_register();
	
	if(old_register){
		lhs_symbol_entry->free_register(old_register);
	}

	lhs_symbol_entry->update_register(result_register);

	
	return *assign_stmt;
}
////////////////////////////////////////////////////////////////

Goto_Ast::Goto_Ast(int bbnum,  int line)
{
	bb = bbnum;
	node_data_type = void_data_type;
	lineno = line;
}

Goto_Ast::~Goto_Ast()
{
	
}

int Goto_Ast :: get_bb(){
	return bb;
}

void Goto_Ast::print(ostream & file_buffer)
{
		file_buffer << AST_SPACE <<"Goto statement:\n";
		file_buffer << AST_NODE_SPACE << "Successor: "<<bb;
	
}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
		Eval_Result & result = *new Eval_Result_Value_Goto();
        result.set_value(bb);
        print(file_buffer);
//        file_buffer << "\n" << AST_SPACE << "GOTO (BB "<<bb<<")";
//		return result;
	    file_buffer << AST_SPACE << "GOTO (BB "<<bb<<")";
        return result;
}

Code_For_Ast & Goto_Ast::compile()
{
	
	//Register_Descriptor* result_reg  = machine_dscr_object.get_new_register();
	string val = static_cast<ostringstream*>( &(ostringstream() << bb) )->str();
	Ics_Opd* goto_reg_opd = new Const_Opd<string>("label" + val);
	Icode_Stmt* jump_statement = new UCJump_IC_Stmt(goto_reg_opd);
	
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	ic_list.push_back(jump_statement);

	Code_For_Ast * goto_stmt;
	if (ic_list.empty() == false)
		goto_stmt = new Code_For_Ast(ic_list, NULL);

	return *goto_stmt;
}

Code_For_Ast & Goto_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	return compile();
}
////////////////////////////////////////////////////////////////

IfElse_Ast::IfElse_Ast(Ast * cond, Goto_Ast * temp_ifGoto, Goto_Ast* temp_elseGoto, int line)
{
	condition = cond;
	ifGoto = temp_ifGoto;
	elseGoto = temp_elseGoto;
	ast_num_child = binary_arity;
	node_data_type = void_data_type;
	lineno = line;
}

IfElse_Ast::~IfElse_Ast()
{
	delete condition;
	delete ifGoto;
	delete elseGoto;
}

bool IfElse_Ast::check_ast()
{
	CHECK_INVARIANT((condition != NULL), "Condition of IF statmemtn cannot be null");
	CHECK_INVARIANT((ifGoto != NULL), "Goto of IF statmemtn cannot be null");
	CHECK_INVARIANT((elseGoto != NULL), "Goto of else cannot be null");

	
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

void IfElse_Ast::print(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "If_Else statement:	" ;
	condition->print(file_buffer);
	file_buffer<<"\n";
    file_buffer<<AST_NODE_SPACE<<"True Successor: " <<ifGoto->get_bb();  
    file_buffer<<"\n";
    file_buffer<<AST_NODE_SPACE<<"False Successor: "<<elseGoto->get_bb(); 
}

Eval_Result & IfElse_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
		Eval_Result & result = condition->evaluate(eval_env, file_buffer);
        Eval_Result & ret = *new Eval_Result_Value_Goto();
       // file_buffer<<"here and there : "<<result.get_value()<<endl;
        print(file_buffer);
        
        file_buffer<<"\n";
        if(result.get_int_value() != 0){
			file_buffer<<AST_SPACE<<"Condition True : Goto (BB "<<ifGoto->get_bb()<<")";
			ret.set_value(ifGoto->get_bb());
		}
		else{
			file_buffer<<AST_SPACE<<"Condition False : Goto (BB "<<elseGoto->get_bb()<<")";
			ret.set_value(elseGoto->get_bb());
		}
		
		return ret;
}

Code_For_Ast & IfElse_Ast::compile()
{
	
	Code_For_Ast condition_code = condition->compile();
	Register_Descriptor * condition_reg = condition_code.get_reg();

	condition_reg->set_used_for_expr_result(true);
	
	int if_goto_block= ifGoto->get_bb();
	int else_goto_block = elseGoto->get_bb();
	
	//Register_Descriptor* result_reg  = machine_dscr_object.get_new_register();
	Register_Descriptor* zero_reg = machine_dscr_object.get_register(zero);
	Ics_Opd* condition_reg_opd = new Register_Addr_Opd(condition_reg);
	
	string val = static_cast<ostringstream*>( &(ostringstream() << if_goto_block) )->str();
	Ics_Opd* ifGoto_reg_opd = new Const_Opd<string>("label" + val);
	
	Ics_Opd* zero_reg_opd = new Register_Addr_Opd(zero_reg);
	
	val = static_cast<ostringstream*>( &(ostringstream() << else_goto_block) )->str();
	Ics_Opd* elseGoto_reg_opd = new Const_Opd<string>("label"+ val);

	Icode_Stmt* check_with_zero= new CJump_IC_Stmt(bne,  zero_reg_opd , ifGoto_reg_opd , condition_reg_opd);
	Icode_Stmt* else_jump_statement = new UCJump_IC_Stmt(elseGoto_reg_opd);
	
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (condition_code.get_icode_list().empty() == false)
		ic_list = condition_code.get_icode_list();
	
	ic_list.push_back(check_with_zero);
	ic_list.push_back(else_jump_statement);
	
	condition_reg->set_used_for_expr_result(false);

	Code_For_Ast * if_else_stmt;
	if (ic_list.empty() == false)
		if_else_stmt = new Code_For_Ast(ic_list, NULL);

	
	return *if_else_stmt;
}

Code_For_Ast & IfElse_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	//TODO : call lra optimize somehow
	//Apparently unnecessary
/*
	if((typeid(*rhs) == typeid(Number_Ast<int>)) || (typeid(*rhs) == typeid(Name_Ast)))
		lra.optimize_lra(mc_2r, NULL, condition);
	//Don't know what to do here, According to base_code , this should seg_fault if condition is Name_ast or Number_ast.
*/

	Code_For_Ast condition_code = condition->compile_and_optimize_ast(lra);
	Register_Descriptor * condition_reg = condition_code.get_reg();

	condition_reg->set_used_for_expr_result(true);
	
	int if_goto_block= ifGoto->get_bb();
	int else_goto_block = elseGoto->get_bb();
	
	//Register_Descriptor* result_reg  = machine_dscr_object.get_new_register();
	Register_Descriptor* zero_reg = machine_dscr_object.get_register(zero);
	Ics_Opd* condition_reg_opd = new Register_Addr_Opd(condition_reg);
	
	string val = static_cast<ostringstream*>( &(ostringstream() << if_goto_block) )->str();
	Ics_Opd* ifGoto_reg_opd = new Const_Opd<string>("label" + val);
	
	Ics_Opd* zero_reg_opd = new Register_Addr_Opd(zero_reg);
	
	val = static_cast<ostringstream*>( &(ostringstream() << else_goto_block) )->str();
	Ics_Opd* elseGoto_reg_opd = new Const_Opd<string>("label"+ val);

	Icode_Stmt* check_with_zero= new CJump_IC_Stmt(bne,  zero_reg_opd , ifGoto_reg_opd , condition_reg_opd);
	Icode_Stmt* else_jump_statement = new UCJump_IC_Stmt(elseGoto_reg_opd);
	
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (condition_code.get_icode_list().empty() == false)
		ic_list = condition_code.get_icode_list();
	
	ic_list.push_back(check_with_zero);
	ic_list.push_back(else_jump_statement);
	
	condition_reg->set_used_for_expr_result(false);

	Code_For_Ast * if_else_stmt;
	if (ic_list.empty() == false)
		if_else_stmt = new Code_For_Ast(ic_list, NULL);

	
	return *if_else_stmt;
}


////////////////////////////////////////////////////////
UnaryExpression_Ast :: UnaryExpression_Ast(Ast *_exp , Expression_Ast::OperatorType _op, int line){
	exp = _exp;
	node_data_type = exp->get_data_type();
	op  = _op;
	lineno = line;
}

void UnaryExpression_Ast :: printOperator(ostream& file_buffer,Expression_Ast::OperatorType op){
    
    switch(op){
        case Expression_Ast::UMINUS: file_buffer<<"UMINUS";break;
	}       
}
UnaryExpression_Ast :: ~UnaryExpression_Ast(){

}

 Data_Type UnaryExpression_Ast :: get_data_type(){
	return node_data_type;
}


void UnaryExpression_Ast :: print(ostream & file_buffer){
	file_buffer <<"\n";	
    //exp->printOperator(file_buffer,op);   //## TO-DO
	file_buffer << AST_NODE_SPACE;
	file_buffer << "Arith: "; 
    printOperator(file_buffer,op);
    file_buffer <<"\n";
    file_buffer << AST_NODE_SPACE<< "LHS (";
	exp->print(file_buffer);
	file_buffer << ")";
}	
	
	

void  UnaryExpression_Ast :: print_value(Local_Environment & eval_env, ostream & file_buffer){
	//file_buffer << replace(evaluate(eval_env , file_buffer).get_value());
}


Eval_Result & UnaryExpression_Ast ::  evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
}

Code_For_Ast & UnaryExpression_Ast::compile(){
	CHECK_INVARIANT((exp != NULL), "Lhs cannot be null");
	
	Code_For_Ast & exp_code= exp->compile();

	Register_Descriptor * exp_result_reg = exp_code.get_reg();

	exp_result_reg->set_used_for_expr_result(true);
	
	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (exp_code.get_icode_list().empty() == false)
		ic_list = exp_code.get_icode_list();

	Register_Descriptor *result_reg;
	if(node_data_type == float_data_type || node_data_type== double_data_type){
		
		result_reg = machine_dscr_object.get_new_float_register();
	}
	else{
		
		result_reg= machine_dscr_object.get_new_register();
	}
	result_reg->set_used_for_expr_result(true);
	
	//cout<<"free register allocated "<<result_reg->get_name()<<"\n";
	
	Ics_Opd* exp_result_opd = new Register_Addr_Opd(exp_result_reg); 	
	Ics_Opd* result_reg_opd = new Register_Addr_Opd(result_reg); 	

	//generate new code to perform the operation
	Icode_Stmt * expression_icode_stmt;
	if(node_data_type == float_data_type || node_data_type == double_data_type ){
		expression_icode_stmt = new Move_IC_Stmt(negf,exp_result_opd,result_reg_opd); 
	}
	else{
		expression_icode_stmt =  new Move_IC_Stmt(neg,exp_result_opd,result_reg_opd);
	}
	ic_list.push_back(expression_icode_stmt);
	
	Code_For_Ast * expression_stmt;
	if (ic_list.empty() == false)
		expression_stmt = new Code_For_Ast(ic_list, result_reg);

	
	exp_result_reg->set_used_for_expr_result(false);
	return *expression_stmt;
	
}


Code_For_Ast & UnaryExpression_Ast::compile_and_optimize_ast(Lra_Outcome & lra){
	CHECK_INVARIANT((exp != NULL), "Lhs cannot be null");
	
	
	if((typeid(*exp) == typeid(Number_Ast<int>)) || 
			 (typeid(*exp) == typeid(Number_Ast<float>)) || 
				(typeid(*exp) == typeid(Name_Ast))){
			lra.optimize_lra(mc_2r, NULL, exp);
	}
	
	Code_For_Ast & exp_code= exp->compile_and_optimize_ast(lra);

	Register_Descriptor * exp_result_reg = exp_code.get_reg();

	exp_result_reg->set_used_for_expr_result(true);
	
	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (exp_code.get_icode_list().empty() == false)
		ic_list = exp_code.get_icode_list();

	Register_Descriptor *result_reg;
	if(node_data_type == float_data_type || node_data_type== double_data_type){
		
		result_reg = machine_dscr_object.get_new_float_register();
	}
	else{
		
		result_reg= machine_dscr_object.get_new_register();
	}
	result_reg->set_used_for_expr_result(true);
	
	//cout<<"free register allocated "<<result_reg->get_name()<<"\n";
	
	Ics_Opd* exp_result_opd = new Register_Addr_Opd(exp_result_reg); 	
	Ics_Opd* result_reg_opd = new Register_Addr_Opd(result_reg); 	

	//generate new code to perform the operation
	Icode_Stmt * expression_icode_stmt;
	if(node_data_type == float_data_type || node_data_type == double_data_type ){
		expression_icode_stmt = new Move_IC_Stmt(negf,exp_result_opd,result_reg_opd); 
	}
	else{
		expression_icode_stmt =  new Move_IC_Stmt(neg,exp_result_opd,result_reg_opd);
	}
	ic_list.push_back(expression_icode_stmt);
	
	Code_For_Ast * expression_stmt;
	if (ic_list.empty() == false)
		expression_stmt = new Code_For_Ast(ic_list, result_reg);

	
	exp_result_reg->set_used_for_expr_result(false);
	return *expression_stmt;
}





////////////////////////////////////////////////////////////////

 Expression_Ast::Expression_Ast(Ast * _lhs, Ast * _rhs,Expression_Ast::OperatorType _op, int line)
{
	lhs = _lhs;
	rhs= _rhs;
	op = _op;
	ast_num_child = binary_arity;
	lineno = line;
	check_ast();
}


Expression_Ast :: Expression_Ast(Ast * _atomic_exp , Data_Type _T, int line){
	lhs = _atomic_exp; 
	rhs = NULL;
	node_data_type = _T;
	ast_num_child = unary_arity;
	lineno = line;
	check_ast();
}

Expression_Ast::~Expression_Ast()
{
	delete lhs;
	delete rhs;
}

bool Expression_Ast::check_ast()
{
	CHECK_INVARIANT((ast_num_child == unary_arity || (rhs != NULL)), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if((ast_num_child == binary_arity) && ((op == GE) || (op == EQ) || (op == NE) || (op == LT) || (op == GT) || (op == LE))){
		node_data_type = int_data_type;
		return true;
	}
	
	if(rhs == NULL){
		assert(ast_num_child == unary_arity);
		return true;
	}
	
	
	
	if ((lhs->get_data_type() == rhs->get_data_type()) || 
			((lhs->get_data_type() == float_data_type || lhs->get_data_type() == double_data_type) &&
			(rhs->get_data_type() == float_data_type || rhs->get_data_type() == double_data_type)))
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Expression statement data type not compatible");
}



void Expression_Ast::print(ostream & file_buffer)
{
	if(rhs == NULL){
		lhs->print(file_buffer);
		return;
	}
    
    file_buffer <<"\n";	
    file_buffer << AST_NODE_SPACE;
	
    
    if((op == GE) || (op == EQ) || (op == NE) || (op == LT) || (op == GT) || (op == LE))
		file_buffer << "Condition: "; 
	else
		file_buffer << "Arith: "; 
	
	
	printOperator(file_buffer , op);
	file_buffer <<"\n";
    
    file_buffer <<  AST_SPACE << "LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";
	file_buffer << "\n" << AST_SPACE<< "RHS (";
	rhs->print(file_buffer);
	file_buffer<<")";
}


void Expression_Ast :: printOperator(ostream& file_buffer,Expression_Ast::OperatorType op){
   
    if(rhs == NULL){
		file_buffer<<"CASTED EXP: NO OP";
		return;
	}
    switch(op){
        case GT: file_buffer<<"GT";break;
        case LT: file_buffer<<"LT";break;
        case LE: file_buffer<<"LE";break;
        case GE: file_buffer<<"GE";break;
        case EQ: file_buffer<<"EQ";break;
        case NE: file_buffer<<"NE";break;
        }
}

Eval_Result & Expression_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
}

Code_For_Ast & Expression_Ast::compile()
{
	
	/* 
		An assignment x = y where y is a variable is 
		compiled as a combination of load and store statements:
		(load) R <- y 
		(store) x <- R
		If y is a constant, the statement is compiled as:
		(imm_Load) R <- y 
		(store) x <- R
		where imm_Load denotes the load immediate operation.
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((ast_num_child == unary_arity || (rhs != NULL)), "Rhs cannot be null");

	Code_For_Ast & lhs_code= lhs->compile();
	Register_Descriptor * lhs_result_reg = lhs_code.get_reg();
	lhs_result_reg->set_used_for_expr_result(true);

	Code_For_Ast rhs_code;
	Register_Descriptor * rhs_result_reg;
	if(rhs!=NULL){
		rhs_code = rhs->compile();
		rhs_result_reg = rhs_code.get_reg();
		rhs_result_reg->set_used_for_expr_result(true);
	}
	
	// Store the statement in ic_list
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_code.get_icode_list().empty() == false)
		ic_list = lhs_code.get_icode_list();

	if (rhs_code.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_code.get_icode_list());
	
	Register_Descriptor *result_reg;
	if(node_data_type == float_data_type || node_data_type== double_data_type){
		
		result_reg = machine_dscr_object.get_new_float_register();
	}
	else{
		
		result_reg= machine_dscr_object.get_new_register();
	}
	result_reg->set_used_for_expr_result(true);
	
	//cout<<"free register allocated "<<result_reg->get_name()<<"\n";
	
	Ics_Opd* lhs_result_opd = new Register_Addr_Opd(lhs_result_reg); 	
	Ics_Opd* result_reg_opd = new Register_Addr_Opd(result_reg); 	
	Ics_Opd* rhs_result_opd;
	Icode_Stmt * expression_icode_stmt=NULL;
	int flag=0;
	if(rhs!=NULL)
		rhs_result_opd = new Register_Addr_Opd(rhs_result_reg); 	
	else{
		
		
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				if ((lhs->get_data_type() == node_data_type)||
					((lhs->get_data_type() == float_data_type || lhs->get_data_type() == double_data_type) &&
					(node_data_type == float_data_type || node_data_type == double_data_type))){
						flag=1;
				}
				else{
					expression_icode_stmt = new Move_IC_Stmt(mtc1,lhs_result_opd,result_reg_opd);
				}
			}
			else{
				if ((lhs->get_data_type() == node_data_type) || 
					((lhs->get_data_type() == float_data_type || lhs->get_data_type() == double_data_type) &&
					(node_data_type == float_data_type || node_data_type == double_data_type)))	{
						flag=1;
				}
				else{
					expression_icode_stmt = new Move_IC_Stmt(mfc1,lhs_result_opd,result_reg_opd);
				}
			}	
		goto label;
	}
	//generate new code to perform the operation
	switch(op){
		case PLUS:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(addf,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(add,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		case MINUS:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(subf,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(sub,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		case MULT:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(mulf,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(mul,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		case DIV:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(divif,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(divi,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		
		case LE:
			expression_icode_stmt= new Compute_IC_Stmt(sle,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case LT:
			expression_icode_stmt= new Compute_IC_Stmt(slt,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case GT:
			expression_icode_stmt= new Compute_IC_Stmt(sgt,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case GE:
			expression_icode_stmt= new Compute_IC_Stmt(sge,lhs_result_opd,rhs_result_opd,result_reg_opd);	
			break;
		case EQ:
			expression_icode_stmt= new Compute_IC_Stmt(seq,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case NE:
			expression_icode_stmt= new Compute_IC_Stmt(sne,lhs_result_opd,rhs_result_opd,result_reg_opd);	
			break;
	}
	
label:	
	if(expression_icode_stmt !=NULL)
		ic_list.push_back(expression_icode_stmt);
	
	lhs_result_reg->set_used_for_expr_result(false);
	if(rhs!=NULL){
		rhs_result_reg->set_used_for_expr_result(false);
	}
	
	if(flag == 1){
		result_reg->set_used_for_expr_result(false);
		result_reg = lhs_result_reg;
		result_reg->set_used_for_expr_result(true);
	}
	
	Code_For_Ast * expression_stmt;
	if (ic_list.empty() == false)
		expression_stmt = new Code_For_Ast(ic_list, result_reg);

	
	return *expression_stmt;
}


Code_For_Ast & Expression_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((ast_num_child == unary_arity || (rhs != NULL)), "Rhs cannot be null");

//TODO	???????

	if((typeid(*lhs) == typeid(Number_Ast<int>)) || 
			 (typeid(*lhs) == typeid(Number_Ast<float>)) || 
				(typeid(*lhs) == typeid(Name_Ast))){
			lra.optimize_lra(mc_2r, NULL, lhs);
	}
	
	
	Code_For_Ast & lhs_code= lhs->compile_and_optimize_ast(lra);
	Register_Descriptor * lhs_result_reg = lhs_code.get_reg();
	lhs_result_reg->set_used_for_expr_result(true);
	
	
	
	Code_For_Ast rhs_code;
	Register_Descriptor * rhs_result_reg;
	
	if(rhs!=NULL){
		if((typeid(*rhs) == typeid(Number_Ast<int>)) || 
			 (typeid(*rhs) == typeid(Number_Ast<float>)) ||
			 (typeid(*rhs) == typeid(Number_Ast<double>)) || 
				(typeid(*rhs) == typeid(Name_Ast)))
			lra.optimize_lra(mc_2r, NULL, rhs);
		rhs_code = rhs->compile_and_optimize_ast(lra);
		rhs_result_reg = rhs_code.get_reg();
		rhs_result_reg->set_used_for_expr_result(true);
	}


	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_code.get_icode_list().empty() == false)
		ic_list = lhs_code.get_icode_list();

	if (rhs_code.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_code.get_icode_list());

	Register_Descriptor *result_reg;
	if(node_data_type == float_data_type || node_data_type== double_data_type){
		
		result_reg = machine_dscr_object.get_new_float_register();
	}
	else{
		
		result_reg= machine_dscr_object.get_new_register();
	}
	result_reg->set_used_for_expr_result(true);
	
	//cout<<"free register allocated "<<result_reg->get_name()<<"\n";
	
	Ics_Opd* lhs_result_opd = new Register_Addr_Opd(lhs_result_reg); 	
	Ics_Opd* rhs_result_opd = new Register_Addr_Opd(rhs_result_reg); 	
	Ics_Opd* result_reg_opd = new Register_Addr_Opd(result_reg); 	

	//generate new code to perform the operation
	Icode_Stmt * expression_icode_stmt=NULL;
	int flag=0;
	if(rhs!=NULL)
		rhs_result_opd = new Register_Addr_Opd(rhs_result_reg); 	
	else{
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				if ((lhs->get_data_type() == node_data_type)||
					((lhs->get_data_type() == float_data_type || lhs->get_data_type() == double_data_type) &&
					(node_data_type == float_data_type || node_data_type == double_data_type))){
						flag=1;
				}
				else{
					expression_icode_stmt = new Move_IC_Stmt(mtc1,lhs_result_opd,result_reg_opd);
				}
			}
			else{
				if ((lhs->get_data_type() == node_data_type) || 
					((lhs->get_data_type() == float_data_type || lhs->get_data_type() == double_data_type) &&
					(node_data_type == float_data_type || node_data_type == double_data_type)))	{
						flag=1;
				}
				else{
					expression_icode_stmt = new Move_IC_Stmt(mfc1,lhs_result_opd,result_reg_opd);
				}
			}	
		goto label;
	}
	//generate new code to perform the operation
	switch(op){
		case PLUS:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(addf,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(add,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		case MINUS:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(subf,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(sub,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		case MULT:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(mulf,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(mul,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		case DIV:
			if(node_data_type == float_data_type || node_data_type == double_data_type){
				expression_icode_stmt= new Compute_IC_Stmt(divif,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			else{
				expression_icode_stmt= new Compute_IC_Stmt(divi,lhs_result_opd,rhs_result_opd,result_reg_opd);
			}
			break;
		
		case LE:
			expression_icode_stmt= new Compute_IC_Stmt(sle,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case LT:
			expression_icode_stmt= new Compute_IC_Stmt(slt,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case GT:
			expression_icode_stmt= new Compute_IC_Stmt(sgt,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case GE:
			expression_icode_stmt= new Compute_IC_Stmt(sge,lhs_result_opd,rhs_result_opd,result_reg_opd);	
			break;
		case EQ:
			expression_icode_stmt= new Compute_IC_Stmt(seq,lhs_result_opd,rhs_result_opd,result_reg_opd);
			break;
		case NE:
			expression_icode_stmt= new Compute_IC_Stmt(sne,lhs_result_opd,rhs_result_opd,result_reg_opd);	
			break;
	}
	
label:	
	
	if(expression_icode_stmt !=NULL)
		ic_list.push_back(expression_icode_stmt);
	
	lhs_result_reg->set_used_for_expr_result(false);
	
	if(rhs!=NULL){
		rhs_result_reg->set_used_for_expr_result(false);
	}
	
	if(flag == 1){
		result_reg->set_used_for_expr_result(false);
		result_reg = lhs_result_reg;
		result_reg->set_used_for_expr_result(true);
	}
	
	Code_For_Ast * expression_stmt;
	expression_stmt = new Code_For_Ast(ic_list, result_reg);
	return *expression_stmt;
}
/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	variable_symbol_entry = &var_entry;

	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");

	ast_num_child = zero_arity;
	node_data_type = variable_symbol_entry->get_data_type();
	lineno = line;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	return *variable_symbol_entry;
}

void Name_Ast::print(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_symbol_entry->get_variable_name();
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	Eval_Result * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		CHECK_INVARIANT(loc_var_val->get_result_enum() == int_result, "Result type can only be int");
		file_buffer << loc_var_val->get_int_value() << "\n";
	}

	else
	{
		CHECK_INVARIANT(glob_var_val->get_result_enum() == int_result, 
			"Result type can only be int and float");

		if (glob_var_val == NULL)
			file_buffer << "0\n";
		else
			file_buffer << glob_var_val->get_int_value() << "\n";
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	if (eval_env.does_variable_exist(variable_name))
	{
		CHECK_INPUT_AND_ABORT((eval_env.is_variable_defined(variable_name) == true), 
					"Variable should be defined before its use", lineno);

		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	CHECK_INPUT_AND_ABORT((interpreter_global_table.is_variable_defined(variable_name) == true), 
				"Variable should be defined before its use", lineno);

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result * i;
	string variable_name = variable_symbol_entry->get_variable_name();

	if (variable_symbol_entry->get_data_type() == int_data_type)
		i = new Eval_Result_Value_Int();
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (result.get_result_enum() == int_result)
	 	i->set_value(result.get_int_value());
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

Code_For_Ast & Name_Ast::compile()
{
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Descriptor * result_register;
	Ics_Opd * register_opd;
	Icode_Stmt * load_stmt;
	if(node_data_type==float_data_type || node_data_type == double_data_type){
		 result_register=  machine_dscr_object.get_new_float_register();
		 register_opd = new Register_Addr_Opd(result_register);

		 load_stmt = new Move_IC_Stmt(loadf, opd, register_opd);

	}
	else{
		 result_register=  machine_dscr_object.get_new_register();
		 register_opd = new Register_Addr_Opd(result_register);

		 load_stmt = new Move_IC_Stmt(load, opd, register_opd);

	}
	 
	
	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	CHECK_INVARIANT((store_register != NULL), "Store register cannot be null");

	Ics_Opd * register_opd = new Register_Addr_Opd(store_register);
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	
	
	
	Icode_Stmt * store_stmt ;
	if(node_data_type == float_data_type || node_data_type==double_data_type){
		
		store_stmt = new Move_IC_Stmt(storef, register_opd, opd);
	}
	else{
		store_stmt = new Move_IC_Stmt(store, register_opd, opd);
	}

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(store_stmt);

	Code_For_Ast & name_code = *new Code_For_Ast(ic_list, store_register);

	return name_code;
}

Code_For_Ast & Name_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	bool load_needed = lra.is_load_needed();

	Register_Descriptor * result_register = lra.get_register();
	CHECK_INVARIANT((result_register != NULL), "Register cannot be null");
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = NULL;
	if (load_needed)
	{
		Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

		if(node_data_type == int_data_type)
			load_stmt = new Move_IC_Stmt(load, opd, register_opd);
		else
			load_stmt = new Move_IC_Stmt(loadf, opd, register_opd);	
			
		ic_list.push_back(load_stmt);
	}

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	constant = number;
	node_data_type = constant_data_type;
	ast_num_child = zero_arity;
	lineno = line;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	file_buffer << std::fixed;
	file_buffer << std::setprecision(2);

	file_buffer << "Num : " << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}

}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	Register_Descriptor * result_register; 
	
	if(node_data_type == int_data_type){
		result_register = machine_dscr_object.get_new_register();
	}
	else{
		result_register = machine_dscr_object.get_new_float_register();
	}
	
	
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(result_register);
	
	Ics_Opd * opd;
	Icode_Stmt * load_stmt;
	
	
	if(node_data_type == int_data_type){
		opd = new Const_Opd<int>(constant);
		load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);
	}
	else{
		opd = new Const_Opd<float>(constant);
		load_stmt = new Move_IC_Stmt(imm_loadf, opd, load_register);
	}
			
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, result_register);

	return num_code;
}


template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lra.get_register() != NULL), "Register assigned through optimize_lra cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(lra.get_register());
	
	Ics_Opd * opd;
	Icode_Stmt * load_stmt;

	if(node_data_type == int_data_type){
		opd = new Const_Opd<int>(constant);
		load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);
	}
	else{
		opd = new Const_Opd<float>(constant);
		load_stmt = new Move_IC_Stmt(imm_loadf, opd, load_register);
	}

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);
	
	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, lra.get_register());

	return num_code;
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(int line)
{
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Int();
	return result;
}

Code_For_Ast & Return_Ast::compile()
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

Code_For_Ast & Return_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

template class Number_Ast<int>;
template class Number_Ast<float>;
