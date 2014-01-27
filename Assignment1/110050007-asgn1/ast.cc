/*********************************************************************************

	            Abstract Syntax Trees for cs306 2013-14
                    Implemented by 
                         - Tanu Kanvar (tanu@cse.iitb.ac.in), and
                         - Uday Khedker (uday@cse.iitb.ac.in) 

*********************************************************************************/

#include<iostream>
#include<fstream>
#include <cstdlib>
#include <stdlib.h>
#include<list>
#include<string>
using namespace std;

#include"ast.hh"

Assignment_Ast::Assignment_Ast(Ast * arg_lhs, Ast * arg_rhs)
{
	lhs = arg_lhs;
	rhs = arg_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

void Assignment_Ast::print(ostream& out){
    lhs->print(out);
    out<<" = ";
    rhs->print(out);
    return;
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name)
{
	variable_name = name;
}

Name_Ast::~Name_Ast()
{
    delete this;
}


void Name_Ast::print(ostream &out){
    out<<variable_name;    
}
///////////////////////////////////////////////////////////////////////////////

Number_Ast::Number_Ast(int number)
{
	constant = number;
}

Number_Ast::~Number_Ast()
{
     delete this;
}

void Number_Ast::print(ostream &out){
    out<<constant;
    
}

////////////////////////////////////////////////////////////////////////////////

Relational_Expr_Ast::Relational_Expr_Ast(Ast* args_lhs,string args_op,Ast* args_rhs)
{
	lhs = args_lhs;
    rhs = args_rhs;
    op = args_op;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
     delete this;
}

void Relational_Expr_Ast::print(ostream &out){
    out<<"(";
    lhs->print(out);
    out<<" "<<op<<" ";
    rhs->print(out);
    out<<")";
}
//////////////////////////////////////////////////////////////////////////////////

Boolean_Expr_Ast::Boolean_Expr_Ast(Ast* args_lhs,string args_op,Ast* args_rhs)
{
	lhs = args_lhs;
    rhs = args_rhs;
    op = args_op;
}

Boolean_Expr_Ast::~Boolean_Expr_Ast()
{
    delete this;
}

void Boolean_Expr_Ast::print(ostream &out){
    out<<"(";
   
     if(rhs == NULL){
        out<<op;
        lhs->print(out);
        
    }
    
    else{
        lhs->print(out);
        out<<op;
        rhs->print(out);
    }
     out<<")";
}


//////////////////////////////////////////////////////////////////////////////////

void print_Ast_List(ast_List_Ptr alist)
{
	list<ast_Ptr>::iterator i;

	int line=1;

	for (i=alist->begin(); i != alist->end(); i++)
        {
		cout << line++ << ": ";
        	(*i)->print(cout);
		cout << "\n";
        }

	cout << "\n";
}


