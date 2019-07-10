// TODO header here
//
// TEST program for Execute (unparsing of SQL statements)

#include <iostream>
#include <string>

// Hyrise SQL parser
#include "SQLParser.h"
#include "SQLParserResult.h"

// Class to test
#include "Execute.h"



// List of test SQL queries
std::string queries[] = {
	// 'Must pass' tests from canvas
	"create table foo (a text, b integer, c double)",
	"select * from foo left join goober on foo.x=goober.x",
	"select * from foo as f left join goober on f.x = goober.x",
	"select * from foo as f left join goober as g on f.x = g.x",
	"select a,b,g.c from foo as f, goo as g",
	"select a,b,c from foo where foo.b > foo.c + 6",
	"select f.a,g.b,h.c from foo as f join goober as g on f.id = g.id where f.z >1",
	// This one should fail
	"foo bar blaz",
	// Additional tests, some of these should fail
	"select f.a, f.b from foo as f", // alias in table name w/o join
	"select * from foo as f", // This one was causing segfaults, not sure why it isnt now ... 
	"select a, b from foo", // most simple type of query
	"select a+b-12.72 from foo", // simple expression with floating point, should work
	"create table foo", // without columns, should FAIL
	"select a.b as Ba, c.d as Cd from foo", // should show 'AS'
	"select a, b from foo where a = c and b > d or a = d", // many part expression, should work
	"select c, d from (select c, d from foo where c > d) as Fuu JOIN bar ON Fuu.c = bar.c" // Complex query that _should_ work
};
int queriesCount = 14; // TODO: REMEMBER TO UPDATE THIS WHEN ADDING TESTS TO ARRAY

// Test routine
int main() {

	std::cout << std::endl;
	for(int i = 0; i < queriesCount; ++i) {
		
		std::cout << "Query " << i << ": " << queries[i] << std::endl;

		// parse with HSQL parser
		hsql::SQLParserResult* result = hsql::SQLParser::parseSQLString(queries[i]);			
		
		// Unparse valid queries with Execute
		std::string unparsed;
		if(result->isValid()) {
			unparsed = Execute::getString(result);
			std::cout << unparsed << std::endl;
		}
		else {
			std::cout << "Invalid SQL: " << queries[i] << std::endl;
		}
		
		std::cout << std::endl;
	}
	
	return 0;
}
