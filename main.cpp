// "Project Butterfly" - CPSC 5300 / 4300 Summer 2019                                                  
// See README.md for details 
// THIS FILE: Program entry point: set up DB environemnt then go to SQL prompt

#include <iostream>
#include <string>

#include "db_cxx.h" // Berkeley
#include "SQLParser.h" // Hyrise
#include "SQLParserResult.h"

#include "Berkeley.h"
#include "Execute.h"

// Global setup parameters
const unsigned int DB_BLOCK_SIZE = 4096;
const char* DB_FILENAME = "Butterfly_1.db"; // Hardcoded filename!

int main(int argc, char* argv[]) {

	// Get filepath for db file from command line argument
	std::string filepath;
	switch(argc) {
	case 1:
		std::cout << "Error: please specifiy database file path" << std::endl;
		return -1;
	case 2:
		filepath = argv[1];
		break;
	case 3:
		std::cout << "Error: unxecpected arguments" << std::endl;
		return -1;
	}

	// Set up DB environment
	Berkeley berk(filepath);
	if(berk.dbIsOk)
		std::cout << "running with database environment at " << filepath << std::endl;
	else {
		std::cout << "ERROR setting up database environment: " << std::endl;
		std::cout << berk.dbErrors << std::endl;
		return -1;
	}
	
    // Main SQL prompt loop
    std::string sql;
    do{
		sql = "";
      std::cout << "SQL> ";
      std::getline(std::cin,sql); // not cin >> because need to get including spaces!
      hsql::SQLParserResult* result;
      result = hsql::SQLParser::parseSQLString(sql);
      if(result->isValid())
			std::cout << Execute::getString(result) << std::endl;
    } while(sql != "quit");
	return 0;
}
