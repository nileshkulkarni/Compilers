#include "common.h"

Expression_Ast::OperatorType getOpertaor(int op){
    swtich(op){
        case NE:
            return Expression_Ast::OperatorType::NE;
        case LE:
            return Expression_Ast::OperatorType::LE;
        case GE:
            return Expression_Ast::OperatorType::GE;
        case GT:
            return Expression_Ast::OperatorType::GT;
        case LT:
            return Expression_Ast::OperatorType::LT;
        case EQ:
            return Expression_Ast::OperatorType::EQ;
    }

}
