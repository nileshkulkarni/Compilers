
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

#ifndef PROCEDURE_HH
#define PROCEDURE_HH

#define replace(r) ((r.data_type == double_data_type)? r.double_ret : (r.data_type == float_data_type)? r.float_ret : r.int_ret)

#include<string>
#include<map>
#include<list>
#include<set>
#include "user-options.hh"
#include "error-display.hh"
#define PROC_SPACE "   "
#define LOC_VAR_SPACE "      "

using namespace std;

extern std::set<int> bbNo;
extern std::set<int> gotoNo;

typedef struct {
    Data_Type data_type;
    string name; 

} parameter;

typedef list<parameter> parameter_list;


class Procedure;

class Procedure
{
	Data_Type return_type;
	string name;
	Symbol_Table local_symbol_table;
	Symbol_Table parameter_symbol_table;
	list<Basic_Block *> basic_block_list;
    set<int> bb;
    set<int> gotoNo;
    
public:
	Procedure(Data_Type proc_return_type, string proc_name);
	~Procedure();

	string get_proc_name();
	void set_basic_block_list(list<Basic_Block *> bb_list);
	void set_local_list(Symbol_Table & new_list);
	void set_parameter_list(Symbol_Table & new_list);

	Data_Type get_return_type();
	void set_return_type(Data_Type );
	Symbol_Table_Entry & get_symbol_table_entry(string variable_name);

 //   void append_local_list(Symbol_Table & new_list); // appends the current procedure's symbol table with the new symbol table
	void print_ast(ostream & file_buffer);
	bool check_for_undefined_blocks();

    bool check_parameter_list(Symbol_Table& new_list);
	
	void add_goto(int bb1);
	void add_bb(int bb1);
	
	Basic_Block * get_next_bb(Basic_Block & current_bb);
	Basic_Block *get_bb_by_number(int n);
	Basic_Block & get_start_basic_block();
	
	Eval_Result & evaluate(ostream & file_buffer);
	Eval_Result & evaluate(ostream & file_buffer , list<Eval_Result_Ret> arguments);
	
	bool variable_in_symbol_list_check(string variable);
};

#endif
