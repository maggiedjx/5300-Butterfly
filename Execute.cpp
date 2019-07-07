// TODO our header here
//
//
//
//See the ref .h file
//https://github.com/klundeen/sql-parser/blob/cpsc4300/src/SQLParserResult.h
// TODO: wish that provided an iterator for statement items!

#include "Execute.h"

std::string Execute::getString(hsql::SQLParserResult* parseTree) {
	
	if(!parseTree->isValid())
		return "ERROR: failure to parse";

	// String to build up with output from unparsing each statment
	std::string output;

	// Iterate over items in the parseTree (stored internally as a std::vector)
	// (should only be one item unless multiple queries seperated by ';' TODO is this true? ...
	size_t length = parseTree->size();
	for(size_t i = 0; i < length; ++i) {
		
			// statment object is parent class for various statement types class
			const hsql::SQLStatement* statement = parseTree->getStatement(i);
		
			// Determine statement type: for now only CREATE TABLE and SELECT
			// Cast to correct type and call appropriate unpare function
			hsql::StatementType stateType = statement->type();
			if(stateType == hsql::kStmtCreate) {
				output += unparseCreate((hsql::CreateStatement*) statement);
			}
			else if(stateType == hsql::kStmtSelect) {
				output += unparseSelect((hsql::SelectStatement*) statement);
			}
			// TODO: add support for additional query types (e.g. DROP TABLE)
			else
				output += "ERROR: Statment type not supported";

			// Seperate output from each query
			output += "\n";
	}

	return output;

}

std::string Execute::unparseCreate(hsql::CreateStatement* statement) {
	return "This is a create statement";
}

std::string Execute::unparseSelect(hsql::SelectStatement* statement) {
	return "This is a select statement";
}
