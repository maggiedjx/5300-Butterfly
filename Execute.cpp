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
		return "ERROR";

	// String to build up
	std::string output = "<";

	// Iterate over items in the parseTree (stored internally as a std::vector)
	size_t length = parseTree->size();
	for(size_t i = 0; i < length; ++i) {
		
			// statment object is parent class for various types class
			const hsql::SQLStatement* statement = parseTree->getStatement(i);
		
			// TODO test number of items in tree with dummy string
			output += "[";
			output += std::to_string(i);
			output += "]"; // TODO 'best practice' when doing string concats ?
	}

	return output;

}
