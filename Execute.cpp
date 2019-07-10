// "Project Butterfly" - CPSC 5300 / 4300 Summer 2019
// See README.md for details
// THIS FILE: Implementation for Execute class (query unparsing)

#include "Execute.h"

std::string Execute::getString(hsql::SQLParserResult* parseTree) {
	
	// Malformed SQL queries will produce 'invalid' SQLParserResult structs
	if(!parseTree->isValid())
		return "ERROR: failure to parse";

	// String to build up with output from unparsing each statment
	std::string output;

	// Iterate over items in the parseTree (stored internally as a std::vector)
	// _Should_ only be one item unless multiple queries seperated by ';' (?)
	size_t length = parseTree->size();
	for(size_t i = 0; i < length; ++i) {
		
			// seperate multiple queries with newline
			if(i > 0)
				output += "\n";

			// statment object is parent class for various statement types class
			const hsql::SQLStatement* statement = parseTree->getStatement(i);
		
			// Determine statement type by checking the Type enum: for now 
			// only CREATE TABLE and SELECT types are supported (NOT INSERT or 
			// DROP statements)
			// Cast to correct type and call appropriate unpare function
			hsql::StatementType stateType = statement->type();
			if(stateType == hsql::StatementType::kStmtCreate) {
				output = unparseCreate((hsql::CreateStatement*) statement);
			}
			else if(stateType == hsql::StatementType::kStmtSelect) {
				output = unparseSelect((hsql::SelectStatement*) statement);
			}
			// Would add support for additional statement types here
			else
				output += "ERROR: Statment type not supported";
	}
	return output;
}

std::string Execute::unparseCreate(hsql::CreateStatement* statement) {
	
	// string to build with statement
	std::string output;

	// What type of object is being created - enum contained inside hsql CreateStatement struct
	hsql::CreateStatement::CreateType type = statement->type;
	if(type == hsql::CreateStatement::kTable)
		output += "CREATE TABLE "; // NOTE: the IF NOT EXISTS options is ignored 
	else {
		// Unsported creations - add here, maybe should throw an exception ...
		output = "ERROR: Unsupported create type (index or view)";
		return output;
	}

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
		else // the hsql parser also has an 'unknown' data type
			output += "OTHER_TYPE";

		// comma seperation, not on last column
		if(i < statement->columns->size() - 1)
			output += ", ";
	}

	// End of column list
	output += ")";
		
	// Note that key constratint information is NOT supported (yet?)
	return output;
}

std::string Execute::unparseSelect(hsql::SelectStatement* statement) {

	std::string output = "SELECT ";

	// list of columns to select, where each is an expression (Expr*), adding commas as needed
	for(size_t i = 0; i < statement->selectList->size(); ++i) {
		output += unparseExpr(statement->selectList->at(i));
		if(i < statement->selectList->size() - 1)
			output += ", ";	
	}

	output += " FROM ";
	
	// Get the referenced table, which may be a table name or even another SELECT statment
	// have to decide what to do based on what it is (may need to recurse!)
	if(statement->fromTable->type == hsql::TableRefType::kTableName)
		output += statement->fromTable->getName();
	// selecting from a JOIN of two tables
	else if(statement->fromTable->type == hsql::TableRefType::kTableJoin) {
		output += "JOIN_TODO";
	}
	// selecting from the results of a nested SELECT (note extraction of select
	// statement type from TableRef type
	else if(statement->fromTable->type == hsql::TableRefType::kTableSelect)
		output += unparseSelect((hsql::SelectStatement*)statement->fromTable);
	// TableRefType enum also has TableCrossProduct option
	else
		output += "TABLE_SRC_NOT_SUPPORTED";
	
	// Get selection condtion (WHERE clause) if any
	if(statement->whereClause != NULL) {
		output += " WHERE ";
		// WHERE condition is an expression
		output += unparseExpr(statement->whereClause);
	}
	
	return output;
}

std::string Execute::unparseExpr(hsql::Expr* expr) {

	std::string output;

	// Check if the expression contains a left and right side 
	// sub-expression, e.g. "a + b" is the expression "+" with left a and right b
	// To unparse these get operator, then _recusivley_ unparse each
	// left and right side expressions
	// NOTE: TODO: what about case of unary expressions? Hyrise has NOT, MINUS, ...
	// Would need to change && to ||, and add individual checks for expr and expr2
	if(expr->expr != NULL && expr->expr2 != NULL) {
		// Left side sub-expression
		output += unparseExpr(expr->expr);
		// Expression character (e.g. "+")
		// "trivial" type operators: single characters
		if(expr->opType == hsql::Expr::OperatorType::SIMPLE_OP) {
			output += " ";
			output += expr->opChar;
			output += " ";
		}
		// 'Non-trival' operators NOTE TODO only binary for now!
		else {
			if(expr->opType == hsql::Expr::OperatorType::NOT_EQUALS)
				output += " <> ";
			else if(expr->opType == hsql::Expr::OperatorType::LESS_EQ)
				output += " <= ";
			else if(expr->opType == hsql::Expr::OperatorType::GREATER_EQ)
				output += " >= ";
			else if(expr->opType == hsql::Expr::OperatorType::AND)
				output += " AND ";
			else if(expr->opType == hsql::Expr::OperatorType::OR)
				output += " OR ";
			// TODO implement additional operators as needed
			// see hyrise source Expr.h for full list!
			else
				output += "_OP_NOT_SUPPORTED_"; 
		}
		// Right side sub-expression
		output += unparseExpr(expr->expr2);
	}
	// Expression is wildcard (*) TODO what is the "correct" way to test this - is it just
	// an opChar of '*' without left & right expressions - no, because OpType is none...???
	else if(expr->getName() == NULL && expr->opType == hsql::Expr::OperatorType::NONE &&
				!expr->isLiteral())
		output = "*";
	// if literal value just return the value 
	// need to decide between returning the floating point and long long int
	// dosn't seem to be any indication in the stuct (???) so just test for zero
	else if(expr->isLiteral()) {
		// use int when float is zero (thus possibly equivilent)
		if(expr->fval == 0.0)
			output += std::to_string(expr->ival);
		else
			output += std::to_string(expr->fval);
	}
	else {
		// Get the table name (if any)
		if(expr->hasTable()) {
			output += expr->table;
			output += ".";
		}
		// Get the column name (already tested existance above)
		output += expr->getName();
		// Get alias (if any)
		if(expr->hasAlias()) {
			output += " AS ";
			output += expr->alias;
		}
	}
	return output;
}

















