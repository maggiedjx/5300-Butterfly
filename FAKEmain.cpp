// Main for ButterflyShell
// (early incomplete version)

#include <iostream>
#include <string> // TODO .h ?
#include <string.h>
#include <exception>

// Note no db_xx include, inside berkeley...

#include "SQLParser.h" // TODO Makefile
#include "SQLParserResult.h" // TODO same as above


#include "Execute.h"
#include "Berkeley.h"

// Global setup parameters
const unsigned int DB_BLOCK_SIZE = 4096;
const char* DB_FILENAME = "Butterfly_1.db";


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


    // TODO Create a Berkeley DB environment in that directory using DbEnv::open with the DB_CREATE flag

    // TODO Have a user-input (do-while) loop prompting the user with "SQL> "
	std::cout << ">";
	std::string userInput;
	//	std::cin >> userInput; // only gets untill space, need to get entire line !
	std::getline(std::cin,userInput);
	std::cout << "parsing string:" << userInput << std::endl;


    // Parse user input with SQLParser::parseSQLString, validate resulting parse tree,
	hsql::SQLParserResult* result = hsql::SQLParser::parseSQLString(userInput);
	if(result->isValid()) {
		std::string resultStr = Execute::getString(result);
		std::cout << "Results: " << std::endl << resultStr << std::endl;
	}
	else
		std::cout << "Invalid parse results..." << std::endl;
	


    // if valid, pass to "execute" and print resulting string to console
	return 0;

}
