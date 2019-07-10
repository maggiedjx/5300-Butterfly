// "Project Butterfly" - CPSC 5300 / 4300 Summer 2019
// See README.md for details
// THIS FILE: Header for Execute class (query unparsing)

#ifndef EXECUTE_H
#define EXECUTE_H

#include <string>

// SQL parser from Hyrise
#include "SQLParser.h"

// For unparsing (not at this point actually executing) the SQLParserResult
// stucts that are produced by the Hyrise SQL parser
// (Defined in /src/SQLParserResult.h)
// This class only contains static methods, and only one public method
// (TODO) In a future milestone the functions in this class, if still needed,
// should probably be moved to a more appropriate place
class Execute {
public:

	// Unparse (convert to string) a hsql::SQLParserResult, which is a struct
	// returned by the Hyrise SQL parser
	// This function DOES NOT support all possible valid SQLParserResult struct,
	// only those resulting for realitivly simple SQL queries
	// See TEST_Execute.cpp for a demonstation of which queries are supported
	// or build test program with "make queryTest"
	static std::string getString(hsql::SQLParserResult* parseTree);

private:
	
	// Unparse (convert to string) CREATE sql statments as stored in
	// the hsq::CreateStatment struc.
	// This DOES NOT unparse all possible valid CreateStatment structs,
	// only basic table and column definitions.  Key contstrains in tables,
	// and the creation of Views or Indicies are NOT supported.
	static std::string unparseCreate(hsql::CreateStatement* statement);

	// Unparese SELECT sql statments as stored in the hsql::SelectStatement
	// This may be called recursively if a SELECT calls another SELECT
	// This DOES NOT unparse all possible valid SelectStatment structs
	// No support for GROUP BY / aggregation (e.g. AVG) or ORDER BY
	static std::string unparseSelect(hsql::SelectStatement* statement);
	
	// Unparse expressions as stored in the hsql::Expr struct
	// Expressions are used within SELECT statments
	// This DOES NOT unparse all possible valid Expr structs,
	// only the simple cases that are needed for passing milestone 1 tests
	static std::string unparseExpr(hsql::Expr* expr);

};

#endif
