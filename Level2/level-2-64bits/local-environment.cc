
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors (theory and lab)  at IIT
           Bombay.

           Release  date Jan  15, 2013.  Copyrights reserved  by Uday
           Khedker. This implemenation has been made available purely
           for academic purposes without any warranty of any kind.

           Documentation  (functionality,   manual, and  design)  and
           related tools are at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#include<string>
#include<fstream>

using namespace std;

#include"local-environment.hh"
#include"error-display.hh"
#include"user-options.hh"
#include <iomanip>

Eval_Result_Ret Eval_Result::get_value()
{
	report_internal_error("Should not reach, Eval_Result : get_value");
}


void Eval_Result::set_value(Eval_Result_Ret number)
{
	report_internal_error("Should not reach, Eval_Result : set_value");
}

bool Eval_Result::is_variable_defined()
{
	report_internal_error("Should not reach, Eval_Result : is_variable_defined");
}

void Eval_Result::set_variable_status(bool def)
{
	report_internal_error("Should not reach, Eval_Result : set_variable_status");
}

///////////////////////////////////////////////////////////////////////////////

Eval_Result_Value_Int::Eval_Result_Value_Int()
{
//	value = Eval_Result_Ret(int_data_type);
	value.data_type = int_data_type;
	defined = false;
	result_type = int_result;
}

Eval_Result_Value_Int::~Eval_Result_Value_Int()
{ }

void Eval_Result_Value_Int::set_value(int number)
{
	value.int_ret = number;
	defined = true;
}

void Eval_Result_Value_Int::set_value(Eval_Result_Ret number)
{
	value.int_ret = number.int_ret;
	assert(number.data_type == int_data_type);
	defined = true;
}
Eval_Result_Ret Eval_Result_Value_Int::get_value()
{
	return value;
}

void Eval_Result_Value_Int::set_variable_status(bool def)
{
	defined = def;
}

bool Eval_Result_Value_Int::is_variable_defined()
{
	return defined;
}

void Eval_Result_Value_Int::set_result_enum(Result_Enum res)
{
	result_type = res;
}

Result_Enum Eval_Result_Value_Int::get_result_enum()
{
	return result_type;
}



///////////////////////////////////////////////////////////////////////////////

Eval_Result_Value_Float::Eval_Result_Value_Float()
{	
	
	value.data_type = float_data_type;
	defined = false;
	result_type = float_result;
}

Eval_Result_Value_Float::~Eval_Result_Value_Float()
{ }

void Eval_Result_Value_Float::set_value(Eval_Result_Ret number)
{
	value.float_ret = number.float_ret;
	assert(number.data_type == float_data_type);
	defined = true;
}

void Eval_Result_Value_Float::set_value(float number)
{
	value.float_ret = number;
	defined = true;
}

Eval_Result_Ret Eval_Result_Value_Float::get_value()
{
	return value;
}

void Eval_Result_Value_Float::set_variable_status(bool def)
{
	defined = def;
}

bool Eval_Result_Value_Float::is_variable_defined()
{
	return defined;
}

void Eval_Result_Value_Float::set_result_enum(Result_Enum res)
{
	result_type = res;
}

Result_Enum Eval_Result_Value_Float::get_result_enum()
{
	return result_type;
}





///////////////////////////////////////////////////////////////////////////////

Eval_Result_Value_Double::Eval_Result_Value_Double()
{
	value.double_ret = 0.0;
	value.data_type = double_data_type;
	defined = false;
	result_type = double_result;
}

Eval_Result_Value_Double::~Eval_Result_Value_Double()
{ }
void Eval_Result_Value_Double::set_value(Eval_Result_Ret number)
{
	value.double_ret = number.double_ret;
	assert(number.data_type == double_data_type);
	defined = true;
}

void Eval_Result_Value_Double::set_value(double number)
{
	value.double_ret = number;
	defined = true;
}

Eval_Result_Ret Eval_Result_Value_Double::get_value()
{
	return value;
}

void Eval_Result_Value_Double::set_variable_status(bool def)
{
	defined = def;
}

bool Eval_Result_Value_Double::is_variable_defined()
{
	return defined;
}

void Eval_Result_Value_Double::set_result_enum(Result_Enum res)
{
	result_type = res;
}

Result_Enum Eval_Result_Value_Double::get_result_enum()
{
	return result_type;
}


///////////////////////////////////////////////////////////////////////////////



Eval_Result_Value_Goto::Eval_Result_Value_Goto()
{
	value.int_ret = 0;
	value.data_type = int_data_type;
	defined = false;
	result_type = goto_result;
}

Eval_Result_Value_Goto::~Eval_Result_Value_Goto()
{ }

void Eval_Result_Value_Goto::set_value(Eval_Result_Ret number)
{
	value.int_ret = number.int_ret;
	defined = true;
}
void Eval_Result_Value_Goto::set_value(int number)
{
	value.int_ret = number;
	defined = true;
}

Eval_Result_Ret Eval_Result_Value_Goto::get_value()
{
	return value;
}

void Eval_Result_Value_Goto::set_variable_status(bool def)
{
	defined = def;
}

bool Eval_Result_Value_Goto::is_variable_defined()
{
	return defined;
}

void Eval_Result_Value_Goto::set_result_enum(Result_Enum res)
{
	result_type = res;
}

Result_Enum Eval_Result_Value_Goto::get_result_enum()
{
	return result_type;
}










///////////////////////////////////////////////////////////////////////////////

Eval_Result_Value_Return::Eval_Result_Value_Return()
{
	result_type = return_result;
}

Eval_Result_Value_Return::~Eval_Result_Value_Return()
{ }


void Eval_Result_Value_Return::set_variable_status(bool def)
{
	defined = def;
}

bool Eval_Result_Value_Return::is_variable_defined()
{
	return defined;
}
void Eval_Result_Value_Return::set_value(Eval_Result_Ret num)
{
	
}

Eval_Result_Ret Eval_Result_Value_Return::get_value()
{
	Eval_Result_Ret res;
	res.data_type = return_data_type;
	return res;
}
void Eval_Result_Value_Return::set_result_enum(Result_Enum res)
{
	result_type = res;
}

Result_Enum Eval_Result_Value_Return::get_result_enum()
{
	return result_type;
}
























///////////////////////////////////////////////////////////////////////////////////

Local_Environment::Local_Environment()
{}

Local_Environment::~Local_Environment()
{}
void Local_Environment::printFormatted(ostream & file_buffer , Eval_Result_Ret R){
	file_buffer<<std::setprecision(2) << std::fixed;
	if(R.data_type == int_data_type)
		file_buffer<<R.int_ret; 
	
	else if(R.data_type == float_data_type)
		file_buffer<<R.float_ret; 
	
	else if(R.data_type == double_data_type)
		file_buffer<<setprecision(4)<<R.double_ret; 
}

void Local_Environment::print(ostream & file_buffer)
{
	map<string, Eval_Result_Value *>::iterator i;
	for (i = variable_table.begin(); i != variable_table.end(); i++)
	{
		Eval_Result_Value * vi = variable_table[(*i).first];
		if (vi != NULL)
		{
			if (vi->is_variable_defined() == false)
				file_buffer << VAR_SPACE << (*i).first << " : undefined" << "\n";
		
			else{
				file_buffer << VAR_SPACE << (*i).first << " : "; 
				printFormatted(file_buffer, vi->get_value());
				file_buffer<< "\n";
			}
		}
	}
}

bool Local_Environment::is_variable_defined(string name)
{
	Eval_Result_Value * i = variable_table[name];
//	cout<<"KKKKKKKKKKKKK : "<<name<< " " <<(i==NULL)<<endl;
	if (i != NULL)
		return i->is_variable_defined();
	else
		return false;
}

Eval_Result_Value * Local_Environment::get_variable_value(string name)
{
	Eval_Result_Value * i = variable_table[name];
	return i;
}

void Local_Environment::put_variable_value(Eval_Result_Value & i, string name)
{
	variable_table[name] = &i;
}

bool Local_Environment::does_variable_exist(string name)
{
	if (variable_table[name] == NULL)
		return false;
	else
		return true;
}
