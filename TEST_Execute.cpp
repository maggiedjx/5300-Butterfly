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
	"create table foo (a text, b integer, c double)",
	"select * from foo left join goober on foo.x=goober.x",
	"select * from foo as f left join goober on f.x = goober.x",
	"select * from foo as f left join goober as g on f.x = g.x",
	"select a,b,g.c from foo as f, goo as g",
	"select a,b,c from foo where foo.b > foo.c + 6",
	"select f.a,g.b,h.c from foo as f join goober as g on f.id = g.id where f.z >1",
	"foo bar blaz"
};
int queriesCount = 8;

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
