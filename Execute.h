// TODO our header here


#ifndef EXECUTE_H
#define EXECUTE_H


#include <string>

// Include SQL parser that was adapted from Hyrise
#include "SQLParser.h"

class Execute {
public:

	// convert the parse tree from Hyrise back into a string
	static std::string getString(hsql::SQLParserResult* parseTree);

private:
	
	// unparse CREATE statements
	static std::string unparseCreate(hsql::CreateStatement* statement);

	// unparse SELECT statments
	static std::string unparseSelect(hsql::SelectStatement* statement);

	// unparse hsql expressions (used inside SELECT statements)
	// TODO: note that this is NOT complete: only those expressions needed for very
	// basic select statements (as shown in Milesone 1 example) are implemented
	static std::string unparseExpr(hsql::Expr* expr);

};




#endif
