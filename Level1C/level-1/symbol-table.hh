
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

#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include<string>
#include<list>

using namespace std;
#define GLOB_SPACE "\t"
#define GLOB_VAR_SPACE "\t"
class Symbol_Table;
class Symbol_Table_Entry;

typedef enum
{
	void_data_type,
	int_data_type
} Data_Type;

typedef enum
{
	global,
	local
} Table_Scope;

class Symbol_Table
{
	list<Symbol_Table_Entry *> variable_table;
	Table_Scope scope;
public:
	Symbol_Table();
	~Symbol_Table();

	Table_Scope get_table_scope();
	void set_table_scope(Table_Scope list_scope);

	void push_symbol(Symbol_Table_Entry * variable);

	bool variable_in_symbol_list_check(string variable);
	Symbol_Table_Entry & get_symbol_table_entry(string variable_name);
	void global_list_in_proc_map_check(int line);
	void generate_code(ostream &file_buffer);
	void create(Local_Environment & local_global_variables_table);
	int get_symbol_table_size();
	int get_position(string var);
};

class Symbol_Table_Entry
{
	string variable_name;
	Data_Type variable_data_type;

public:
	Symbol_Table_Entry();
	Symbol_Table_Entry(string & name, Data_Type new_data_type);
	~Symbol_Table_Entry();

	Data_Type get_data_type();
	string get_variable_name();
};

#endif
