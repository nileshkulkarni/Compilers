
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

#include<string>
#include<fstream>
#include<iostream>

using namespace std;

#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"
std::set<int> bbNo;
std::set<int> gotoNo;

Procedure::Procedure(Data_Type proc_return_type, string proc_name)
{
	return_type = proc_return_type;
	name = proc_name;
}

Procedure::~Procedure()
{
	list<Basic_Block *>::iterator i;
	for (i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		delete (*i);
}

string Procedure::get_proc_name()
{
	return name;
}
bool Procedure::check_for_undefined_blocks(std::set<int> bb, std::set<int> gotoNo){
    std::set<int>::iterator it;
    for(it = gotoNo.begin();it!=gotoNo.end();it++){
        if(bb.find(*it) == bb.end()){
	        string file_name = command_options.get_file_name();
            char error_string[100];
            sprintf(error_string,"%s :: cfglp error : bb %d doesn't exist",file_name.c_str(),*it);
            print_error(string(error_string),1); 
            return false;
        
        }

    }

    return true;
}

void Procedure::set_basic_block_list(list<Basic_Block *> bb_list)
{
	basic_block_list = bb_list;
}
bool Procedure::check_parameter_list(Symbol_Table& new_list){
    local_symbol_table_list = local_symbol_table.get_symbol_table_list();
    new_symbol_table_list = new_symbol_table_list.get_symbol_table_list();

    symbol_table_list::iterator it_local = local_symbol_table_list;
    symbol_table_list::iterator it_new = new_symbol_table_list.begin();

    for(;it_local!=local_symbol_table_list.end() || it_new !=new_symbol_table_list.end();it_local++,it_new++){
        //should match at every instant
        if(((*it_local)->get_variable_name() == (*it_new)->get_variable_name()) && ((*it_local)->get_data_type() == (*it_new)->get_data_type())){
            continue;
        return false;
    }

    if(it_local==local_symbol_table_list && it_new == new_symbol_table_list.end()){
        return true;
    }
    return false;
}
void Procedure::append_local_list(Symbol_Table & new_list)
{
   local_symbol_table.append_local_list(new_list); 

}
void Procedure::set_local_list(Symbol_Table & new_list)
{
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);
}

Data_Type Procedure::get_return_type()
{
	return return_type;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	return local_symbol_table.variable_in_symbol_list_check(variable);
}

Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name)
{
	return local_symbol_table.get_symbol_table_entry(variable_name);
}

void Procedure::print_ast(ostream & file_buffer)
{
	file_buffer << PROC_SPACE << "Procedure: main" << "\n"; //CHANGE
	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		(*i)->print_bb(file_buffer);
}
	
Basic_Block & Procedure::get_start_basic_block()
{
	list<Basic_Block *>::iterator i;
	i = basic_block_list.begin();
	return **i;
}


Basic_Block * Procedure::get_bb_by_number(int n){

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == n)
		return (*i);
	}
	return NULL;
}





Basic_Block * Procedure::get_next_bb(Basic_Block & current_bb)
{
	bool flag = false;

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == current_bb.get_bb_number())
		{
			flag = true;
			continue;
		}
		if (flag)
			return (*i);
	}
	
	return NULL;
}


Eval_Result & Procedure::evaluate(ostream & file_buffer)
{
	Local_Environment & eval_env = *new Local_Environment();
	local_symbol_table.create(eval_env);
	
	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure " << name << "\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{	file_buffer<<"\n";
		result = &(current_bb->evaluate(eval_env, file_buffer));
		
		
		if(result->get_result_enum() == return_result)
			break;
		
		if(result->get_result_enum() == goto_result){
			assert(result->is_variable_defined());
			current_bb = get_bb_by_number(replace(result->get_value())); 	
		}
		else
			current_bb = get_next_bb(*current_bb);		
	}

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating):\n";
	eval_env.print(file_buffer);

	return *result;
}

