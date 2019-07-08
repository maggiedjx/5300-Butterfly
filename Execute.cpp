// TODO our header here
//
//
//
//See the ref .h file
// sql-parser/src/SQLParserResult.h

#include "Execute.h"

std::string Execute::getString(hsql::SQLParserResult* parseTree) {
	
	if(!parseTree->isValid())
		return "ERROR: failure to parse"; // Exception? TODO

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
	
	// string to build with statement
	std::string output;

	// What type of object is being created - enum contained inside hsql CreateStatement struct
	hsql::CreateStatement::CreateType type = statement->type;
	if(type == hsql::CreateStatement::kTable)
		output += "CREATE TABLE ";
	else {
		output += "ERROR: Unsupported create type (index or view)";
		return output;
		// TODO throw exception here?
	}

	// TODO: IF NOT EXISTS modifier...

	// What is the name of the table being created?
	output += statement->tableName;

	// Start of column list
	output += " (";

	// Get the list of column names and data types
	for(size_t i = 0; i < statement->columns->size(); ++i) {
		
		// Get column name
		output += statement->columns->at(i)->name;
		output += " ";

		// Get column data type
		hsql::ColumnDefinition::DataType colType = statement->columns->at(i)->type;
		if(colType == hsql::ColumnDefinition::INT)
			output += "INT";
		else if(colType == hsql::ColumnDefinition::TEXT)
			output += "TEXT";
		else if(colType == hsql::ColumnDefinition::DOUBLE)
			output += "DOUBLE";
		else
			output += "OTHER_TYPE";

		// comma seperation, not on last column
		if(i < statement->columns->size() - 1)
			output += ", ";
	}

	// End of column list
	output += ")";
		

	// TODO note ignoring key info and internal SelectStatement* for now!
	return output;
}

std::string Execute::unparseSelect(hsql::SelectStatement* statement) {
	
	return "This is a select statement";
}
