
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

#ifndef LOCAL_ENVIRONMENT_HH
#define LOCAL_ENVIRONMENT_HH

#include<string>
#include<map>
#include "symbol-table.hh"
#include <cassert>
#define VAR_SPACE "         "
#define replace(r) ((r.data_type == double_data_type)? r.double_ret : (r.data_type == float_data_type)? r.float_ret : r.int_ret)



struct Eval_Result_Ret{
	int int_ret;
	float float_ret;
	double double_ret;
	Data_Type data_type;
	
	Eval_Result_Ret(){
		int_ret =-1;
	}
	
};




using namespace std;

typedef enum
{
	int_result,
	float_result,
	double_result,
	void_result,
	goto_result,
	return_result
} Result_Enum;



class Eval_Result;
class Local_Environment;



class Eval_Result 
{
protected:
	Result_Enum result_type;

public:
	virtual Eval_Result_Ret get_value();
	virtual void set_value(Eval_Result_Ret value) = 0;
	


	virtual bool is_variable_defined();
	virtual void set_variable_status(bool def);

	virtual void set_result_enum(Result_Enum res) = 0;
	virtual Result_Enum get_result_enum() = 0;
};


class Eval_Result_Value:public Eval_Result
{
public:
	virtual void set_value(Eval_Result_Ret number) = 0;
	virtual Eval_Result_Ret get_value() = 0;

	virtual bool is_variable_defined() = 0;
	virtual void set_variable_status(bool def) = 0;

	virtual void set_result_enum(Result_Enum res) = 0;
	virtual Result_Enum get_result_enum() = 0;
};




class Eval_Result_Value_Int:public Eval_Result_Value
{
	Eval_Result_Ret value;
	bool defined;
public:
	Eval_Result_Value_Int();
	~Eval_Result_Value_Int();

	void set_value(Eval_Result_Ret number);
	void set_value(int number);
	Eval_Result_Ret get_value();

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
};


class Eval_Result_Value_Float:public Eval_Result_Value
{
	Eval_Result_Ret value;
	bool defined;
public:
	Eval_Result_Value_Float();
	~Eval_Result_Value_Float();

	void set_value(Eval_Result_Ret number);
	void set_value(float number);
	Eval_Result_Ret get_value();

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
};


class Eval_Result_Value_Double:public Eval_Result_Value
{
	Eval_Result_Ret value;
	bool defined;
public:
	Eval_Result_Value_Double();
	~Eval_Result_Value_Double();

	void set_value(Eval_Result_Ret number);
	void set_value(double number);
	
	Eval_Result_Ret get_value();

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
};









class Eval_Result_Value_Goto:public Eval_Result_Value
{
	Eval_Result_Ret value;
	bool defined;
public:
	Eval_Result_Value_Goto();
	~Eval_Result_Value_Goto();

	void set_value(Eval_Result_Ret number);
	void set_value(int number);
	
	
	Eval_Result_Ret get_value();

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
};






class Eval_Result_Value_Return:public Eval_Result_Value
{
	Eval_Result_Ret value;
	bool defined;
public:
	Eval_Result_Value_Return();  //makes a void_data_type
	~Eval_Result_Value_Return();

	void set_value(Eval_Result_Ret number);
	Eval_Result_Ret get_value();

	void set_variable_status(bool def);
	bool is_variable_defined();

	void set_result_enum(Result_Enum res);
	Result_Enum get_result_enum();
};






class Local_Environment
{
	map<string, Eval_Result_Value *> variable_table;

public:
	Local_Environment();
	~Local_Environment();

	void print(ostream & file_buffer);
	bool is_variable_defined(string name);
	Eval_Result_Value * get_variable_value(string name);
	void put_variable_value(Eval_Result_Value & value, string name);
	bool does_variable_exist(string name);
	void printFormatted(ostream & file_buffer , Eval_Result_Ret R);
};

extern Local_Environment interpreter_global_table;

#endif
