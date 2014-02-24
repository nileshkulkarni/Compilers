
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

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}






void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}


/*
void Assignment_Ast::set_data_type(Data_Type data_type)
{
	node_data_type=data_type;
}
*/


Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);
	
	lhs->print_value(eval_env, file_buffer);
	
	return result;
}
/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << loc_var_val->get_value() << "\n";
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value() << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name))
	{
		
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>

Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
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
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
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






///////////////////////////////////////////////////////////////////////////
IfElse_Ast ::IfElse_Ast(Ast * _condition	 , Goto_Ast * if_Goto, Goto_Ast * else_Goto){
	condition = _condition;
	ifGoto = if_Goto;
	elseGoto = else_Goto;
}


IfElse_Ast ::~IfElse_Ast(){
	delete(condition);
	delete(ifGoto);
	delete(elseGoto);
	
}




void IfElse_Ast ::  print_ast(ostream & file_buffer){
	
	file_buffer << AST_SPACE << "If_Else statement:	" ;
	condition->print_ast(file_buffer);
	file_buffer<<"\n";
    file_buffer<<AST_NODE_SPACE<<"True Successor: " <<ifGoto->get_bb();  
    file_buffer<<"\n";
    file_buffer<<AST_NODE_SPACE<<"False Successor: "<<elseGoto->get_bb(); 
}

Eval_Result & IfElse_Ast:: evaluate(Local_Environment & eval_env, ostream & file_buffer){
		Eval_Result & result = condition->evaluate(eval_env, file_buffer);
        Eval_Result & ret = *new Eval_Result_Value_Goto();
        
        
        
       // file_buffer<<"here and there : "<<result.get_value()<<endl;
        
        print_ast(file_buffer);
        
        
        file_buffer<<"\n";
        if(result.get_value() != 0){
			file_buffer<<AST_SPACE<<"Condition True : Goto (BB "<<ifGoto->get_bb()<<")";
			ret.set_value(ifGoto->get_bb());
		}
		else{
			file_buffer<<AST_SPACE<<"Condition False : Goto (BB "<<elseGoto->get_bb()<<")";
			ret.set_value(elseGoto->get_bb());
		}
		
		return ret;
       
}








////////////////////////////////////////////////////////////////////////////////////////////

Goto_Ast ::	Goto_Ast(int _bb){
	bb = _bb;
}

Goto_Ast ::	~Goto_Ast(){}


int Goto_Ast :: get_bb(){
	return bb;
}


void Goto_Ast ::	print_ast(ostream & file_buffer){
	file_buffer << AST_SPACE <<"Goto statement:\n";
	file_buffer << AST_NODE_SPACE << "Successor: "<<bb<<"\n";
};

Eval_Result & Goto_Ast:: evaluate(Local_Environment & eval_env, ostream & file_buffer){
		Eval_Result & result = *new Eval_Result_Value_Goto();
        result.set_value(bb);
        print_ast(file_buffer);
	    file_buffer << AST_SPACE << "GOTO (BB "<<bb<<")";
        return result;
}



/////////////////////////////////////////////////////////////////////////////////////////
Expression_Ast :: Expression_Ast(Ast * _lhs , Ast *  _rhs , OperatorType _op){
	lhs = _lhs;
	node_data_type = lhs->get_data_type();
	rhs = _rhs;
	op  = _op;
}


Expression_Ast :: Expression_Ast(Ast * _atomic_exp , Data_Type _T){
	lhs = _atomic_exp; 
	rhs = NULL;
	node_data_type = _T;
}
	



bool Expression_Ast::check_ast(int line)
{
	if(rhs == NULL){
		return true;
	}
	
	if (lhs->get_data_type() == rhs->get_data_type())
	{
			node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Expression statement data type not compatible", line);
}


Data_Type Expression_Ast::get_data_type() 
{
	return node_data_type;
}

	

void Expression_Ast :: print_ast(ostream & file_buffer){
    file_buffer <<"\n";	
    file_buffer << AST_NODE_SPACE<<"Condition: ";
    printOperator(file_buffer,op);
    file_buffer <<"\n";
    file_buffer << AST_NODE_SPACE<< "LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";
	file_buffer << AST_CONDITION_SPACE<< "RHS (";
	rhs->print_ast(file_buffer);
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
        case PLUS: file_buffer<<"PLUS";break;
        case MINUS: file_buffer<<"MINUS";break;
        case MULT: file_buffer<<"MULT";break;
        case DIV: file_buffer<<"DIV";break;
    }
}



template<class DATA_TYPE>
Eval_Result &Expression_Ast :: evaluate(Local_Environment & eval_env, ostream & file_buffer){
		Eval_Result & result;
		if(node_data_type == float_data_type){
			result = *new Eval_Result_Value_Templated<float>(float_data_type);
		}
		
		else if(node_data_type == double_data_type){
			result = *new Eval_Result_Value_Templated<double>(double_data_type);
		}
		
		else if(node_data_type == int_data_type){
			result = *new Eval_Result_Value_Int();
		}
		
		DATA_TYPE res ;
		DATA_TYPE lVal =(lhs->evaluate(eval_env,file_buffer)).get_value();
		/*
		int res ;
		int lVal =(lhs->evaluate(eval_env,file_buffer)).get_value();
		int rVal =(rhs->evaluate(eval_env,file_buffer)).get_value();
		*/
		
		
		if(rhs == NULL){
				res = (DATA_TYPE)lVal;
		}
		
		else{
			DATA_TYPE rVal =(rhs->evaluate(eval_env,file_buffer)).get_value();
			switch(op){
				case GT:
						res = lVal>rVal;
						break;
				case LT:
						res = lVal <rVal;
						break;
				case EQ:
						res = lVal == rVal;
						break;
				case NE:
						res = lVal !=rVal;
						break;
				case GE:
						res = lVal >=rVal;
						break;
				case LE:
						res = lVal <=rVal;
						break;
				case PLUS:
						res = lVal + rVal;
						break;
				case MINUS:
						res = lVal - rVal;
						break;
				case MULT:
						res = lVal * rVal;
						break;				
				case DIV:
						res = lVal / rVal;
						break;				
				
			}
		}
		
        result.set_value(res);
        return result;
}











///////////////////////////////////////////////////////////////////////////	
UnaryExpression_Ast :: UnaryExpression_Ast(Ast *_exp , Expression_Ast::OperatorType _op){
	exp = _exp;
	node_data_type = exp->get_data_type();
	op  = _op;
}

UnaryExpression_Ast :: ~UnaryExpression_Ast(){

}

 Data_Type UnaryExpression_Ast :: get_data_type(){
	return node_data_type;
}


void UnaryExpression_Ast :: print_ast(ostream & file_buffer){
	file_buffer <<"\n";	
    //exp->printOperator(file_buffer,op);
    file_buffer <<"\n";
    file_buffer << AST_NODE_SPACE<< "LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")";
}	
	
	

UnaryExpression_Ast :: void print_value(Local_Environment & eval_env, ostream & file_buffer){
	file_buffer << evaluate(eval_env , file_buffer).get_value();
}


template<class DATA_TYPE>	
UnaryExpression_Ast :: Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
		Eval_Result & result;
		if(node_data_type == float_data_type){
			result = *new Eval_Result_Value_Templated<float>(float_data_type);
		}
		
		else if(node_data_type == double_data_type){
			result = *new Eval_Result_Value_Templated<double>(double_data_type);
		}
		
		else if(node_data_type == int_data_type){
			result = *new Eval_Result_Value_Int();
		}
		
		DATA_TYPE res ;
		DATA_TYPE lVal =(lhs->evaluate(eval_env,file_buffer)).get_value();
		
		switch(op){
			case PLUS:
					res = lVal;
					break;
			case MINUS:
					res = -lVal;
					break;
		}
	
	
        result.set_value(res);
        return result;
}













//////////////////////////////////////////////////////////////////////////////
Return_Ast::Return_Ast()
{}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Return();
	print_ast(file_buffer);
	return result;
}

template class Number_Ast<int>;
template class Number_Ast<float>;
